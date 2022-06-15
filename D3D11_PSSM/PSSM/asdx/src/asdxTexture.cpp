//-------------------------------------------------------------------------------------------
// File : asdxTexture.cpp
// Desc : Texture Module.
// Copyright(c) Project Asura. All right reserved.
//-------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------
// Includes
//-------------------------------------------------------------------------------------------
#include <asdxTexture.h>
#include <asdxUtil.h>
#include <cstdio>
#include <cassert>


namespace /* anonymous */ {


//-------------------------------------------------------------------------------------------
// Constant Values
//-------------------------------------------------------------------------------------------
static const unsigned int  MAP_CURRENT_VERSION = 0x00000002;


/////////////////////////////////////////////////////////////////////////////////////////////
// MAP_DATA_HEADER structure
/////////////////////////////////////////////////////////////////////////////////////////////
struct MAP_DATA_HEADER
{
    unsigned int    Width;          //!< 画像の横幅です.
    unsigned int    Height;         //!< 画像の縦幅です.
    unsigned int    Depth;          //!< 画像の奥行です.
    unsigned int    Format;         //!< フォーマットです.
    unsigned int    MipMapCount;    //!< ミップマップ数です.
    unsigned int    SurfaceCount;   //!< サーフェイス数です.
};


/////////////////////////////////////////////////////////////////////////////////////////////
// MAP_RESOURCE_DATA structure
/////////////////////////////////////////////////////////////////////////////////////////////
struct MAP_RESOURCE_HEADER
{
    unsigned int    Width;          //!< 横幅です.
    unsigned int    Height;         //!< 縦幅です.
    unsigned int    Pitch;          //!< 1行あたりのバイト数です.
    unsigned int    SlicePitch;     //!< テクセルデータのバイト数です.
};

/////////////////////////////////////////////////////////////////////////////////////////////
// MAP_RESOURCE_DATA structure
/////////////////////////////////////////////////////////////////////////////////////////////
struct MAP_RESOURCE_DATA
{
    MAP_RESOURCE_HEADER header;     //!< リソースヘッダです.
    unsigned char*      pPixels;    //!< ピクセルデータです.
};


/////////////////////////////////////////////////////////////////////////////////////////////
// MAP_FILE_HEADER structure
/////////////////////////////////////////////////////////////////////////////////////////////
struct MAP_FILE_HEADER
{
    unsigned char   Magic[ 4 ];     //!< マジックです( 'M', 'A', 'P', '\0' )
    unsigned int    Version;        //!< ファイルバージョンです.
    unsigned int    DataHeaderSize; //!< データヘッダ構造体のサイズです.
    MAP_DATA_HEADER DataHeader;     //!< データヘッダです.
};


//-------------------------------------------------------------------------------------------
//      デバッグオブジェクト名を設定します.
//-------------------------------------------------------------------------------------------
template<UINT TNameLength> inline
void SetDebugObjectName( ID3D11DeviceChild* resource, const char (&name)[TNameLength] )
{
    #if defined(_DEBUG) || defined(DEBUG)
        resource->SetPrivateData(WKPDID_D3DDebugObjectName, TNameLength - 1, name);
    #else
        UNREFERENCED_PARAMETER(resource);
        UNREFERENCED_PARAMETER(name);
    #endif
}


//-------------------------------------------------------------------------------------------
//! @brief      2次元テクスチャを生成します.
//!
//! @param [in]     pDevice         デバイスです.
//! @param [in]     width           テクスチャの横幅です.
//! @param [in]     height          テクスチャの縦幅です.
//! @param [in]     mipCount        ミップマップ数です.
//! @param [in]     surfaceCount    サーフェイス数です.
//! @param [in]     format          DXGIフォーマットです.
//! @param [in]     usage           使用方法.
//! @param [in]     bindFlags       バインドフラグです.
//! @param [in]     cpuAccessFlags  CPUAアクセスフラグです.
//! @param [in]     miscFlags       その他のオプション.
//! @param [in]     pInitData       サブリソースです.
//! @param [out]    ppTexture       テクスチャです.
//! @param [out]    ppSRV           シェーダリソースビューです.
//! @retval true    生成に成功.
//! @retval false   生成に失敗.
//-------------------------------------------------------------------------------------------
bool CreateTexture2D
(
    ID3D11Device*               pDevice,
    UINT                        width,
    UINT                        height,
    UINT                        mipCount,
    UINT                        surfaceCount,
    DXGI_FORMAT                 format,
    D3D11_USAGE                 usage,
    UINT                        bindFlags,
    UINT                        cpuAccessFlags,
    UINT                        miscFlags,
    D3D11_SUBRESOURCE_DATA*     pInitData,
    ID3D11Texture2D**           ppTexture,
    ID3D11ShaderResourceView**  ppSRV
)
{
    assert( pInitData != nullptr );

    bool isCubeMap = ( surfaceCount == 6 );

    // テクスチャ2Dの設定
    D3D11_TEXTURE2D_DESC td;

    td.Width              = width;
    td.Height             = height;
    td.MipLevels          = mipCount;
    td.ArraySize          = surfaceCount;
    td.Format             = format;
    td.SampleDesc.Count   = 1;
    td.SampleDesc.Quality = 0;
    td.Usage              = usage;
    td.BindFlags          = bindFlags;
    td.CPUAccessFlags     = cpuAccessFlags;
    if ( isCubeMap )
    { td.MiscFlags = miscFlags | D3D11_RESOURCE_MISC_TEXTURECUBE; }
    else
    { td.MiscFlags = miscFlags & ~D3D11_RESOURCE_MISC_TEXTURECUBE; }

    HRESULT hr = S_OK;

    // テクスチャを生成.
    ID3D11Texture2D* pTexture = nullptr;
    hr = pDevice->CreateTexture2D( &td, pInitData, &pTexture );

    // チェック.
    if ( SUCCEEDED( hr ) && ( pTexture != nullptr ) )
    {
        // シェーダリソースビューが指定されている場合.
        if ( ppSRV != nullptr )
        {
            // シェーダリソースビューの設定.
            D3D11_SHADER_RESOURCE_VIEW_DESC sd;

            // ゼロクリア.
            memset( &sd, 0, sizeof( sd ) );

            // 画像フォーマットの設定.
            sd.Format = format;

            // キューブマップの場合.
            if ( isCubeMap )
            {
                sd.ViewDimension = D3D_SRV_DIMENSION_TEXTURECUBE;
                sd.TextureCube.MipLevels = td.MipLevels;
            }
            // 普通の2Dテクスチャの場合.
            else
            {
                sd.ViewDimension       = D3D11_SRV_DIMENSION_TEXTURE2D;
                sd.Texture2D.MipLevels = td.MipLevels;
            }

            // シェーダリソースビューを生成.
            hr = pDevice->CreateShaderResourceView( pTexture, &sd, ppSRV );

            // チェック.
            if ( FAILED( hr ) )
            {
                // テクスチャを解放.
                pTexture->Release();

                // 異常終了.
                return false;
            }
        }

        // テクスチャが指定されている場合.
        if ( ppTexture != nullptr )
        {
            // テクスチャを設定.
            (*ppTexture) = pTexture;
        }
        else
        {
            // デバッグオブジェクト名を設定.
            SetDebugObjectName( pTexture, "asdxTexture2D" );

            // テクスチャを解放.
            pTexture->Release();
        }

        // 正常終了.
        return true;
    }
 

    // 異常終了.
    return false;
}


//-------------------------------------------------------------------------------------------
//! @brief      3次元テクスチャを生成します.
//!
//! @param [in]     pDevice         デバイスです.
//! @param [in]     width           テクスチャの横幅です.
//! @param [in]     height          テクスチャの縦幅です.
//! @param [in]     depth           テクスチャの奥行です.
//! @param [in]     mipCount        ミップマップ数です.
//! @param [in]     format          DXGIフォーマットです.
//! @param [in]     usage           使用方法.
//! @param [in]     bindFlags       バインドフラグです.
//! @param [in]     cpuAccessFlags  CPUAアクセスフラグです.
//! @param [in]     miscFlags       その他のオプション.
//! @param [in]     pInitData       サブリソースです.
//! @param [out]    ppTexture       テクスチャです.
//! @param [out]    ppSRV           シェーダリソースビューです.
//! @retval true    生成に成功.
//! @retval false   生成に失敗.
//-------------------------------------------------------------------------------------------
bool CreateTexture3D
(
    ID3D11Device*               pDevice,
    UINT                        width,
    UINT                        height,
    UINT                        depth,
    UINT                        mipCount,
    DXGI_FORMAT                 format,
    D3D11_USAGE                 usage,
    UINT                        bindFlags,
    UINT                        cpuAccessFlags,
    UINT                        miscFlags,
    D3D11_SUBRESOURCE_DATA*     pInitData,
    ID3D11Texture3D**           ppTexture,
    ID3D11ShaderResourceView**  ppSRV
)
{
    assert( pInitData != nullptr );
    HRESULT hr = S_OK;

    // テクスチャ3Dの設定.
    D3D11_TEXTURE3D_DESC td;

    td.Width          = width;
    td.Height         = height;
    td.Depth          = depth;
    td.MipLevels      = mipCount;
    td.Format         = format;
    td.Usage          = usage;
    td.BindFlags      = bindFlags;
    td.CPUAccessFlags = cpuAccessFlags;
    td.MiscFlags      = miscFlags & ~D3D11_RESOURCE_MISC_TEXTURECUBE;

    // テクスチャを生成.
    ID3D11Texture3D* pTexture = nullptr;
    hr = pDevice->CreateTexture3D( &td, pInitData, &pTexture );

    // チェック.
    if ( SUCCEEDED( hr ) && pTexture != nullptr )
    {
        // シェーダリソースビューが指定されている場合.
        if ( ppSRV != nullptr )
        {
            D3D11_SHADER_RESOURCE_VIEW_DESC sd;

            // ゼロクリア.
            memset( &sd, 0, sizeof(sd) );

            // シェーダリソースビューの設定.
            sd.Format = format;
            sd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE3D;
            sd.Texture3D.MipLevels = td.MipLevels;

            // シェーダリソースビューを生成.
            hr = pDevice->CreateShaderResourceView( pTexture, &sd, ppSRV );

            // エラーチェック.
            if ( FAILED( hr ) )
            {
                // 解放.
                pTexture->Release();

                // 異常終了.
                return false;
            }
        }

        // テクスチャが指定されている場合.
        if ( pTexture != nullptr )
        {
            // テクスチャを設定.
            (*ppTexture) = pTexture;
        }
        else
        {
            // デバッグオブジェクト名を設定.
            SetDebugObjectName( pTexture, "asdxTexture3D" );

            // 解放.
            pTexture->Release();
        }

        // 正常終了.
        return true;
    }

    // 異常終了.
    return false;
}


} // namespace /* anonymous */


namespace asdx {

//////////////////////////////////////////////////////////////////////////////////////////////
// Texture2D class
//////////////////////////////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------------------------
//      コンストラクタです.
//--------------------------------------------------------------------------------------------
Texture2D::Texture2D()
: m_Format  ( FORMAT_INVALID )
, m_pTexture( nullptr )
, m_pSRV    ( nullptr )
{ /* DO_NOTHING */ }

//--------------------------------------------------------------------------------------------
//      デストラクタです.
//--------------------------------------------------------------------------------------------
Texture2D::~Texture2D()
{ /* DO_NOTHING */ }

//--------------------------------------------------------------------------------------------
//      メモリ解放処理です.
//--------------------------------------------------------------------------------------------
void Texture2D::Release()
{
    // テクスチャを解放.
    if ( m_pTexture )
    {
        m_pTexture->Release();
        m_pTexture = nullptr;
    }

    // シェーダリソースビューを解放.
    if ( m_pSRV )
    {
        m_pSRV->Release();
        m_pSRV = nullptr;
    }

    m_Format = FORMAT_INVALID;
}

//--------------------------------------------------------------------------------------------
//      ファイルからテクスチャを生成します.
//--------------------------------------------------------------------------------------------
bool Texture2D::CreateFromFile( ID3D11Device* pDevice, const char* filename )
{
    // NULLチェック.
    if ( pDevice == nullptr )
    {
        // エラーログ出力.
        printf_s( "Error : Invalid Argument.\n" );

        // 異常終了.
        return false;
    }

    // NULLチェック.
    if ( filename == nullptr )
    {
        // エラーログ出力.
        printf_s( "Error : Invalid Argument.\n" );

        // 異常終了.
        return false;
    }

    FILE* pFile;

    // ファイルを開く.
    errno_t err = fopen_s( &pFile, filename, "rb" );

    // ファイルオープンチェック.
    if ( err != 0 )
    {
        // エラーログ出力.
        printf_s( "Error : CreateFromFile() Failed. filename = %s\n", filename );

        // 異常終了.
        return false;
    }

    // ファイルヘッダ読み込み.
    MAP_FILE_HEADER header;
    fread( &header, sizeof( MAP_FILE_HEADER ), 1, pFile );

    // マジックをチェック.
    if ( ( header.Magic[ 0 ] != 'M' )
      || ( header.Magic[ 1 ] != 'A' )
      || ( header.Magic[ 2 ] != 'P' )
      || ( header.Magic[ 3 ] != '\0' ) )
    {
        // エラーログ出力.
        printf_s( "Error : Invalid File.\n" );

        // ファイルを閉じる.
        fclose( pFile );

        // 異常終了.
        return false;
    }

    // バージョンチェック.
    if ( header.Version != MAP_CURRENT_VERSION )
    {
        // エラーログ出力.
        printf_s( "Error : Invalid File Version.\n" );

        // ファイルを閉じる.
        fclose( pFile );

        // 異常終了.
        return false;
    }

    // データヘッダ構造体のサイズをチェック.
    if ( header.DataHeaderSize != sizeof( MAP_DATA_HEADER ) )
    {
        // エラーログ出力.
        printf_s( "Error : Data Header Size Not Matched.\n" );

        // ファイルを閉じる.
        fclose( pFile );

        // 異常終了.
        return false;
    }

    // ミップマップ数を設定.
    const unsigned int mipCount = header.DataHeader.MipMapCount;

    // サーフェイス数を設定.
    const unsigned int surfaceCount = header.DataHeader.SurfaceCount;

    // リソースデータのメモリを確保.
    MAP_RESOURCE_DATA* pResources = new MAP_RESOURCE_DATA[ mipCount * surfaceCount ];

    // NULLチェック.
    if ( pResources == nullptr )
    {
        // エラーログ出力.
        printf_s( "Error : Memory Allocate Failed.\n" );

        // ファイルを閉じる.
        fclose( pFile );

        // 異常終了.
        return false;
    }

    // 各サーフェイスごとに処理.
    for( unsigned int j=0; j<surfaceCount; ++j )
    {
        // 各ミップレベルを取得.
        for( unsigned int i=0; i<mipCount; ++i )
        {
            int idx = (mipCount * j ) + i;

            // リソースデータヘッダの読み込み.
            fread( &pResources[ idx ].header, sizeof( MAP_RESOURCE_HEADER ), 1, pFile );

            // ピクセル数取得.
            size_t numPixels = static_cast< size_t >( pResources[ idx ].header.SlicePitch );
            assert( numPixels != 0 );

            // メモリ確保.
            pResources[ idx ].pPixels = new unsigned char [ numPixels ];

            // NULLチェック.
            if ( pResources[ idx ].pPixels == nullptr )
            {
                // エラーログ出力.
                printf_s( "Error : Memory Allocate Failed. \n" );

                // ファイルを閉じる.
                fclose( pFile );

                // ピクセルデータのメモリを解放.
                for( unsigned int k=0; k<(unsigned int)idx; ++k )
                {
                    delete [] pResources[ k ].pPixels;
                    pResources[ k ].pPixels = nullptr;
                }

                // メモリ解放.
                delete [] pResources;
                pResources = nullptr;

                // 異常終了.
                return false;
            }
        
            // ピクセルデータ読み取り.
            fread( pResources[ idx ].pPixels, sizeof( unsigned char ), numPixels, pFile );
        }
    }

    // ファイルを閉じる.
    fclose( pFile );

    // フォーマットを設定.
    m_Format = static_cast< int >( header.DataHeader.Format );

    // フォーマットを判別.
    DXGI_FORMAT format;
    switch( m_Format )
    {
    case FORMAT_A8:
        { format = DXGI_FORMAT_A8_UNORM; }
        break;

    case FORMAT_L8:
        { format = DXGI_FORMAT_R8_UNORM; }
        break;

    case FORMAT_R8G8B8A8:
        { format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB; }
        break;

    case FORMAT_BC1:
        { format = DXGI_FORMAT_BC1_UNORM_SRGB; }
        break;

    case FORMAT_BC2:
        { format = DXGI_FORMAT_BC2_UNORM_SRGB; }
        break;

    case FORMAT_BC3:
        { format = DXGI_FORMAT_BC3_UNORM_SRGB; }
        break;
    }

    // メモリを確保.
    D3D11_SUBRESOURCE_DATA* pSubRes = new D3D11_SUBRESOURCE_DATA[ mipCount * surfaceCount ];

    // NULLチェック.
    if ( pSubRes == nullptr )
    {
        // エラーログ出力.
        printf_s( "Error : Memory Allocate Failed. \n" );

        // ピクセルデータのメモリ解放.
        for( unsigned int j=0; j<surfaceCount; ++j )
        {
            for( unsigned int i=0; i<mipCount; ++i )
            {
                int idx = ( mipCount * j ) + i;
                delete [] pResources[ idx ].pPixels;
                pResources[ idx ].pPixels = nullptr;
            }
        }

        // メモリを解放.
        delete [] pResources;
        pResources = nullptr;

        // 異常終了.
        return false;
    }

    // サブリソースを設定.
    for( unsigned int j=0; j<surfaceCount; ++j )
    {
        for( unsigned int i=0; i<mipCount; ++i )
        {
            int idx = ( mipCount * j ) + i;
            pSubRes[ idx ].pSysMem          = ( const void* )pResources[ idx ].pPixels;
            pSubRes[ idx ].SysMemPitch      = pResources[ idx ].header.Pitch;
            pSubRes[ idx ].SysMemSlicePitch = pResources[ idx ].header.SlicePitch;
        }
    }

    // テクスチャを生成.
    bool result = CreateTexture2D( 
                        pDevice, 
                        header.DataHeader.Width,
                        header.DataHeader.Height,
                        mipCount, 
                        header.DataHeader.SurfaceCount,
                        format,
                        D3D11_USAGE_DEFAULT,
                        D3D11_BIND_SHADER_RESOURCE,
                        0,
                        0,
                        pSubRes,
                        &m_pTexture,
                        &m_pSRV );

    // 不要になったメモリを解放
    for( unsigned int j=0; j<surfaceCount; ++j )
    {
        for( unsigned int i=0; i<mipCount; ++i )
        {
            int idx = ( mipCount * j ) + i;
            delete [] pResources[ idx ].pPixels;
            pResources[ idx ].pPixels = nullptr;
        }
    }
    delete [] pResources;
    pResources = nullptr;

    delete [] pSubRes;
    pSubRes = nullptr;


    // 処理結果を返却.
    return result;
}

//--------------------------------------------------------------------------------------------
//      フォーマットを取得します.
//--------------------------------------------------------------------------------------------
int Texture2D::GetFormat() const
{ return m_Format; }

//--------------------------------------------------------------------------------------------
//      テクスチャを取得します.
//--------------------------------------------------------------------------------------------
ID3D11Texture2D* Texture2D::GetTexture()
{ return m_pTexture; }

//--------------------------------------------------------------------------------------------
//      シェーダリソースビューを取得します.
//--------------------------------------------------------------------------------------------
ID3D11ShaderResourceView* Texture2D::GetSRV()
{ return m_pSRV; }




//////////////////////////////////////////////////////////////////////////////////////////////
// Texture3D class
//////////////////////////////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------------------------
//      コンストラクタです.
//--------------------------------------------------------------------------------------------
Texture3D::Texture3D()
: m_Format  ( FORMAT_INVALID )
, m_pTexture( nullptr )
, m_pSRV    ( nullptr )
{ /* DO_NOTHING */ }

//--------------------------------------------------------------------------------------------
//      デストラクタです.
//--------------------------------------------------------------------------------------------
Texture3D::~Texture3D()
{ /* DO_NOTHING */ }

//--------------------------------------------------------------------------------------------
//      メモリ解放処理です.
//--------------------------------------------------------------------------------------------
void Texture3D::Release()
{
    // テクスチャを解放.
    if ( m_pTexture )
    {
        m_pTexture->Release();
        m_pTexture = nullptr;
    }

    // シェーダリソースビューを解放.
    if ( m_pSRV )
    {
        m_pSRV->Release();
        m_pSRV = nullptr;
    }

    m_Format = FORMAT_INVALID;
}

//--------------------------------------------------------------------------------------------
//      ファイルからテクスチャを生成します.
//--------------------------------------------------------------------------------------------
bool Texture3D::CreateFromFile( ID3D11Device* pDevice, const char* filename )
{
    // NULLチェック.
    if ( pDevice == nullptr )
    {
        // エラーログ出力.
        printf_s( "Error : Invalid Argument.\n" );

        // 異常終了.
        return false;
    }

    // NULLチェック.
    if ( filename == nullptr )
    {
        // エラーログ出力.
        printf_s( "Error : Invalid Argument.\n" );

        // 異常終了.
        return false;
    }

    FILE* pFile;

    // ファイルを開く.
    errno_t err = fopen_s( &pFile, filename, "rb" );

    // ファイルオープンチェック.
    if ( err != 0 )
    {
        // エラーログ出力.
        printf_s( "Error : CreateFromFile() Failed. filename = %s\n", filename );

        // 異常終了.
        return false;
    }

    // ファイルヘッダ読み込み.
    MAP_FILE_HEADER header;
    fread( &header, sizeof( MAP_FILE_HEADER ), 1, pFile );

    // マジックをチェック.
    if ( ( header.Magic[ 0 ] != 'M' )
      || ( header.Magic[ 1 ] != 'A' )
      || ( header.Magic[ 2 ] != 'P' )
      || ( header.Magic[ 3 ] != '\0' ) )
    {
        // エラーログ出力.
        printf_s( "Error : Invalid File.\n" );

        // ファイルを閉じる.
        fclose( pFile );

        // 異常終了.
        return false;
    }

    // バージョンチェック.
    if ( header.Version != MAP_CURRENT_VERSION )
    {
        // エラーログ出力.
        printf_s( "Error : Invalid File Version.\n" );

        // ファイルを閉じる.
        fclose( pFile );

        // 異常終了.
        return false;
    }

    // データヘッダ構造体のサイズをチェック.
    if ( header.DataHeaderSize != sizeof( MAP_DATA_HEADER ) )
    {
        // エラーログ出力.
        printf_s( "Error : Data Header Size Not Matched.\n" );

        // ファイルを閉じる.
        fclose( pFile );

        // 異常終了.
        return false;
    }

    // ミップマップ数を設定.
    const unsigned int mipCount = header.DataHeader.MipMapCount;

    // サーフェイス数を設定.
    const unsigned int surfaceCount = header.DataHeader.SurfaceCount;

    // リソースデータのメモリを確保.
    MAP_RESOURCE_DATA* pResources = new MAP_RESOURCE_DATA[ mipCount * surfaceCount ];

    // NULLチェック.
    if ( pResources == nullptr )
    {
        // エラーログ出力.
        printf_s( "Error : Memory Allocate Failed.\n" );

        // ファイルを閉じる.
        fclose( pFile );

        // 異常終了.
        return false;
    }

    // 各サーフェイスごとに処理.
    for( unsigned int j=0; j<surfaceCount; ++j )
    {
        // 各ミップレベルを取得.
        for( unsigned int i=0; i<mipCount; ++i )
        {
            int idx = (mipCount * j ) + i;

            // リソースデータヘッダの読み込み.
            fread( &pResources[ idx ].header, sizeof( MAP_RESOURCE_HEADER ), 1, pFile );

            // ピクセル数取得.
            size_t numPixels = static_cast< size_t >( pResources[ idx ].header.SlicePitch );
            assert( numPixels != 0 );

            // メモリ確保.
            pResources[ idx ].pPixels = new unsigned char [ numPixels ];

            // NULLチェック.
            if ( pResources[ idx ].pPixels == nullptr )
            {
                // エラーログ出力.
                printf_s( "Error : Memory Allocate Failed. \n" );

                // ファイルを閉じる.
                fclose( pFile );

                // ピクセルデータのメモリを解放.
                for( unsigned int k=0; k<(unsigned int)idx; ++k )
                {
                    delete [] pResources[ k ].pPixels;
                    pResources[ k ].pPixels = nullptr;
                }

                // メモリ解放.
                delete [] pResources;
                pResources = nullptr;

                // 異常終了.
                return false;
            }
        
            // ピクセルデータ読み取り.
            fread( pResources[ idx ].pPixels, sizeof( unsigned char ), numPixels, pFile );
        }
    }

    // ファイルを閉じる.
    fclose( pFile );

    // フォーマットを設定.
    m_Format = static_cast< int >( header.DataHeader.Format );

    // フォーマットを判別.
    DXGI_FORMAT format;
    switch( m_Format )
    {
    case FORMAT_A8:
        { format = DXGI_FORMAT_A8_UNORM; }
        break;

    case FORMAT_L8:
        { format = DXGI_FORMAT_R8_UNORM; }
        break;

    case FORMAT_R8G8B8A8:
        { format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB; }
        break;

    case FORMAT_BC1:
        { format = DXGI_FORMAT_BC1_UNORM_SRGB; }
        break;

    case FORMAT_BC2:
        { format = DXGI_FORMAT_BC2_UNORM_SRGB; }
        break;

    case FORMAT_BC3:
        { format = DXGI_FORMAT_BC3_UNORM_SRGB; }
        break;
    }

    // メモリを確保.
    D3D11_SUBRESOURCE_DATA* pSubRes = new D3D11_SUBRESOURCE_DATA[ mipCount * surfaceCount ];

    // NULLチェック.
    if ( pSubRes == nullptr )
    {
        // エラーログ出力.
        printf_s( "Error : Memory Allocate Failed. \n" );

        for( unsigned int j=0; j<surfaceCount; ++j )
        {
            // ピクセルデータのメモリ解放.
            for( unsigned int i=0; i<mipCount; ++i )
            {
                int idx = (mipCount * j) + i;
                delete [] pResources[ idx ].pPixels;
                pResources[ idx ].pPixels = nullptr;
            }
        }

        // メモリを解放.
        delete [] pResources;
        pResources = nullptr;

        // 異常終了.
        return false;
    }

    // サブリソースを設定.
    for( unsigned int j=0; j<surfaceCount; ++j )
    {
        for( unsigned int i=0; i<mipCount; ++i )
        {
            int idx = ( mipCount * j ) + i;
            pSubRes[ idx ].pSysMem          = ( const void* )pResources[ idx ].pPixels;
            pSubRes[ idx ].SysMemPitch      = pResources[ idx ].header.Pitch;
            pSubRes[ idx ].SysMemSlicePitch = pResources[ idx ].header.SlicePitch;
        }
    }

    // テクスチャを生成.
    bool result = CreateTexture3D( 
                        pDevice, 
                        header.DataHeader.Width,
                        header.DataHeader.Height,
                        header.DataHeader.Depth,
                        mipCount, 
                        format,
                        D3D11_USAGE_DEFAULT,
                        D3D11_BIND_SHADER_RESOURCE,
                        0,
                        0,
                        pSubRes,
                        &m_pTexture,
                        &m_pSRV );

    // 不要になったメモリを解放
    for( unsigned int j=0; j<surfaceCount; ++j )
    {
        for( unsigned int i=0; i<mipCount; ++i )
        {
            int idx = ( mipCount * j ) + i;
            delete [] pResources[ idx ].pPixels;
            pResources[ idx ].pPixels = nullptr;
        }
    }
    delete [] pResources;
    pResources = nullptr;

    delete [] pSubRes;
    pSubRes = nullptr;


    // 処理結果を返却.
    return result;
}


} // namespace asdx
