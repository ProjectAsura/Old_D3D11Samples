//-------------------------------------------------------------------------------------------------
// File : App.cpp
// Desc : Sample Application.
// Copyright(c) Project Asura. All right reserved.
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
// Includes
//-------------------------------------------------------------------------------------------------
#include <asdxMisc.h>
#include <asdxLogger.h>
#include <asdxResDDS.h>
#include <asdxRenderState.h>
#include <App.h>
#include <cassert>


namespace /* anonymous */ {

//-------------------------------------------------------------------------------------------------
//! @brief      DDSデータからリソーステクスチャを生成します.
//!
//! @param[in]      value       DDSデータ.
//! @return     TGAデータから生成したリソーステクスチャを返却します.
//-------------------------------------------------------------------------------------------------
asdx::ResTexture CreateFromDds( const asdx::ResDDS& value )
{
    asdx::ResTexture result;
    result.Width        = value.GetWidth();
    result.Height       = value.GetHeight();
    result.Depth        = value.GetDepth();
    result.Format       = value.GetFormat();
    result.MipMapCount  = value.GetMipMapCount();
    result.SurfaceCount = value.GetSurfaceCount();

    auto size = value.GetSurfaceCount() * value.GetMipMapCount();
    auto pSubResources = new asdx::SubResource[ size ];
    assert( pSubResources != nullptr );

    auto pSurfaces = value.GetSurfaces();

    bool needModify = false;

    for( u32 i=0; i<size; ++i )
    {
        pSubResources[i].Width      = pSurfaces[i].Width;
        pSubResources[i].Height     = pSurfaces[i].Height;
        pSubResources[i].Pitch      = pSurfaces[i].Pitch;
        pSubResources[i].SlicePitch = pSurfaces[i].SlicePitch;

        switch( result.Format )
        {
        case asdx::DDS_FORMAT_B8G8R8_UNORM:
            {
                pSubResources[i].pPixels = new (std::nothrow) u8 [ pSurfaces[i].Width * pSurfaces[i].Height * 4 ];
                assert( pSubResources[i].pPixels != nullptr );
                for( u32 j=0; j<pSurfaces[i].Width * pSurfaces[i].Height; ++j )
                {
                    pSubResources[i].pPixels[ j * 4 + 2 ] = pSurfaces[i].pPixels[ j * 3 + 0 ];
                    pSubResources[i].pPixels[ j * 4 + 1 ] = pSurfaces[i].pPixels[ j * 3 + 1 ];
                    pSubResources[i].pPixels[ j * 4 + 0 ] = pSurfaces[i].pPixels[ j * 3 + 2 ];
                    pSubResources[i].pPixels[ j * 4 + 3 ] = 255;
                }

                pSubResources[i].Pitch = pSurfaces[i].Width * 4;
                pSubResources[i].SlicePitch = pSubResources[i].Pitch * pSurfaces[i].Height;

                needModify = true;
            }
            break;

        case asdx::DDS_FORMAT_B5G6R5_UNORM:
            {
                pSubResources[i].pPixels = new (std::nothrow) u8 [ pSurfaces[i].Width * pSurfaces[i].Height * 4 ];
                assert( pSubResources[i].pPixels != nullptr );
                auto src = reinterpret_cast<u16*>( pSurfaces[i].pPixels );
                auto dst = reinterpret_cast<u32*>( pSubResources[i].pPixels );
                for( u32 j=0; j<pSurfaces[i].Width * pSurfaces[i].Height; ++j )
                {
                    u16 color = (*src);

                    u32 b = ( ( color & 0xf800 ) >>  8 ) | ( ( color & 0xe000 ) >> 13 );
                    u32 g = ( ( color & 0x07e0 ) <<  5 ) | ( ( color & 0x0600 ) >>  5 );
                    u32 r = ( ( color & 0x001f ) << 19 ) | ( ( color & 0x001c ) << 14 );

                    (*dst) = b | g | r | 0xff000000;

                    src++;
                    dst++;
                }

                pSubResources[i].Pitch = pSurfaces[i].Width * 4;
                pSubResources[i].SlicePitch = pSubResources[i].Pitch * pSurfaces[i].Height;

                needModify = true;
            }
            break;

        case asdx::DDS_FORMAT_B5G5R5A1_UNORM:
            {
                pSubResources[i].pPixels = new (std::nothrow) u8 [ pSurfaces[i].Width * pSurfaces[i].Height * 4 ];
                assert( pSubResources[i].pPixels != nullptr );
                auto src = reinterpret_cast<u16*>( pSurfaces[i].pPixels );
                auto dst = reinterpret_cast<u32*>( pSubResources[i].pPixels );
                for( u32 j=0; j<pSurfaces[i].Width * pSurfaces[i].Height; ++j )
                {
                    u16 color = (*src);

                    u32 b = ( ( color & 0x7c00 ) >>  7 ) | ( ( color & 0x7000 ) >> 12 );
                    u32 g = ( ( color & 0x03e0 ) <<  6 ) | ( ( color & 0x0380 ) >>  1 );
                    u32 r = ( ( color & 0x001f ) << 19 ) | ( ( color & 0x001c ) << 14 );
                    u32 a = ( ( color & 0x8000 ) ? 0xff000000 : 0 );

                    (*dst) = b | g | r | a;

                    src++;
                    dst++;
                }

                pSubResources[i].Pitch = pSurfaces[i].Width * 4;
                pSubResources[i].SlicePitch = pSubResources[i].Pitch * pSurfaces[i].Height;

                needModify = true;
            }
            break;

        case asdx::DDS_FORMAT_B5G5R5X1_UNORM:
            {
                pSubResources[i].pPixels = new (std::nothrow) u8 [ pSurfaces[i].Width * pSurfaces[i].Height * 4 ];
                assert( pSubResources[i].pPixels != nullptr );
                auto src = reinterpret_cast<u16*>( pSurfaces[i].pPixels );
                auto dst = reinterpret_cast<u32*>( pSubResources[i].pPixels );
                for( u32 j=0; j<pSurfaces[i].Width * pSurfaces[i].Height; ++j )
                {
                    u16 color = (*src);

                    u32 b = ( ( color & 0x7c00 ) >>  7 ) | ( ( color & 0x7000 ) >> 12 );
                    u32 g = ( ( color & 0x03e0 ) <<  6 ) | ( ( color & 0x0380 ) >>  1 );
                    u32 r = ( ( color & 0x001f ) << 19 ) | ( ( color & 0x001c ) << 14 );
                    u32 a = 0xff000000;

                    (*dst) = b | g | r | a;

                    src++;
                    dst++;
                }

                pSubResources[i].Pitch = pSurfaces[i].Width * 4;
                pSubResources[i].SlicePitch = pSubResources[i].Pitch * pSurfaces[i].Height;

                needModify = true;
            }
            break;

        case asdx::DDS_FORMAT_B4G4R4A4_UNORM:
            {
                pSubResources[i].pPixels = new (std::nothrow) u8 [ pSurfaces[i].Width * pSurfaces[i].Height * 4 ];
                assert( pSubResources[i].pPixels != nullptr );
                auto src = reinterpret_cast<u16*>( pSurfaces[i].pPixels );
                auto dst = reinterpret_cast<u32*>( pSubResources[i].pPixels );
                for( u32 j=0; j<pSurfaces[i].Width * pSurfaces[i].Height; ++j )
                {
                    u16 color = (*src);

                    u32 b = ( ( color & 0x0f00 ) >>  4 ) | ( ( color & 0x0f00 ) >>  8 );
                    u32 g = ( ( color & 0x00f0 ) <<  8 ) | ( ( color & 0x00f0 ) >>  4 );
                    u32 r = ( ( color & 0x000f ) << 20 ) | ( ( color & 0x000f ) << 16 );
                    u32 a = ( ( color & 0xf000 ) << 16 ) | ( ( color & 0xf000 ) << 12 );

                    (*dst) = b | g | r | a;

                    src++;
                    dst++;
                }

                pSubResources[i].Pitch = pSurfaces[i].Width * 4;
                pSubResources[i].SlicePitch = pSubResources[i].Pitch * pSurfaces[i].Height;

                needModify = true;
            }
            break;

        case asdx::DDS_FORMAT_B4G4R4X4_UNORM:
            {
                pSubResources[i].pPixels = new (std::nothrow) u8 [ pSurfaces[i].Width * pSurfaces[i].Height * 4 ];
                assert( pSubResources[i].pPixels != nullptr );
                auto src = reinterpret_cast<u16*>( pSurfaces[i].pPixels );
                auto dst = reinterpret_cast<u32*>( pSubResources[i].pPixels );
                for( u32 j=0; j<pSurfaces[i].Width * pSurfaces[i].Height; ++j )
                {
                    u16 color = (*src);

                    u32 b = ( ( color & 0x0f00 ) >>  4 ) | ( ( color & 0x0f00 ) >>  8 );
                    u32 g = ( ( color & 0x00f0 ) <<  8 ) | ( ( color & 0x00f0 ) >>  4 );
                    u32 r = ( ( color & 0x000f ) << 20 ) | ( ( color & 0x000f ) << 16 );
                    u32 a = 0xff000000;

                    (*dst) = b | g | r | a;

                    src++;
                    dst++;
                }

                pSubResources[i].Pitch = pSurfaces[i].Width * 4;
                pSubResources[i].SlicePitch = pSubResources[i].Pitch * pSurfaces[i].Height;

                needModify = true;
            }
            break;

        case asdx::DDS_FORMAT_R8G8B8X8_UNORM:
            {
                pSubResources[i].pPixels = new (std::nothrow) u8 [ pSurfaces[i].SlicePitch ];
                assert( pSubResources[i].pPixels != nullptr );
                memcpy( pSubResources[i].pPixels, pSurfaces[i].pPixels, pSurfaces[i].SlicePitch );
                needModify = true;
            }
            break;

        default:
            {
                pSubResources[i].pPixels = new (std::nothrow) u8 [ pSurfaces[i].SlicePitch ];
                assert( pSubResources[i].pPixels != nullptr );
                memcpy( pSubResources[i].pPixels, pSurfaces[i].pPixels, pSurfaces[i].SlicePitch );
            }
            break;
        }
    }

    if ( needModify )
    {
        result.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    }

    result.pResources = pSubResources;

    return result;
}

} // namespace /* anonymous */


///////////////////////////////////////////////////////////////////////////////////////////////////
// App class
///////////////////////////////////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------------------------------------
//      コンストラクタです.
//-------------------------------------------------------------------------------------------------
App::App()
: asdx::Application( L"Sample", 960, 540, nullptr, nullptr, nullptr )
{
}

//-------------------------------------------------------------------------------------------------
//      デストラクタです.
//-------------------------------------------------------------------------------------------------
App::~App()
{
}

//-------------------------------------------------------------------------------------------------
//      初期化処理です.
//-------------------------------------------------------------------------------------------------
bool App::OnInit()
{
    // スプライトの初期化に失敗しました.
    if ( !m_Sprite.Init( m_pDevice.GetPtr(), f32(m_Width), f32(m_Height) ) )
    {
        ELOG( "Error : Sprite Initialize Failed." );
        return false;
    }

    // レンダーステートの初期化.
    if ( !asdx::RenderState::GetInstance().Init( m_pDevice.GetPtr() ) )
    {
        ELOG( "Error : RenderState Initailize Failed. ");
        return false;
    }

    // ファイルパスを探す.
    const char16* loadPath = L"res/sample_bc1.dds";
    //const char16* loadPath = L"res/sample_bc2.dds";
    //const char16* loadPath = L"res/sample_bc3.dds";
    //const char16* loadPath = L"res/sample_A8R8G8B8.dds";
    //const char16* loadPath = L"res/sample_G16R16.dds";
    //const char16* loadPath = L"res/sample_G16R16F.dds";
    //const char16* loadPath = L"res/sample_R16F.dds";
    //const char16* loadPath = L"res/sample_A16B16G16R16F.dds";
    //const char16* loadPath = L"res/sample_A16B16G16R16.dds";
    //const char16* loadPath = L"res/sample_A32B32G32R32F.dds";
    //const char16* loadPath = L"res/sample_R32F.dds";
    //const char16* loadPath = L"res/sample_G32R32F.dds";
    //const char16* loadPath = L"res/sample_X8R8G8B8.dds";
    //const char16* loadPath = L"res/sample_A2B10G10R10.dds";
    //const char16* loadPath = L"res/sample_A8L8.dds";
    //const char16* loadPath = L"res/sample_L8.dds";
    //const char16* loadPath = L"res/sample_L16.dds";
    //const char16* loadPath = L"res/sample_R8G8B8.dds";
    //const char16* loadPath = L"res/sample_R5G6B5.dds";
    //const char16* loadPath = L"res/sample_X1R5G5B5.dds";
    //const char16* loadPath = L"res/sample_A1R5G5B5.dds";
    //const char16* loadPath = L"res/sample_X4R4G4B4.dds";
    //const char16* loadPath = L"res/sample_A4R4G4B4.dds";
    //const char16* loadPath = L"res/sample_X8B8G8R8.dds";
    //const char16* loadPath = L"res/sample_A8B8G8R8.dds";

    std::wstring path;
    if ( !asdx::SearchFilePath( loadPath, path ) )
    {
        ELOG( "Error : File Not Found." );
        return false;
    }

    // BMPをロード.
    asdx::ResDDS dds;
    if ( !dds.Load( path.c_str() ) )
    {
        ELOG( "Error : File Load Failed. path = %s", path.c_str() );
        return false;
    }

    // リソーステクスチャにコンバート.
    asdx::ResTexture res = CreateFromDds( dds );

    // DDSを解放.
    dds.Release();

    // テクスチャデータを生成.
    if ( !m_Texture.Create( m_pDevice.GetPtr(), m_pDeviceContext.GetPtr(), res ) )
    {
        ELOG( "Error : Create Texture Failed." );
        res.Release();
        return false;
    }

    // リソーステクスチャを解放.
    res.Release();

    // 正常終了.
    return true;
}

//-------------------------------------------------------------------------------------------------
//      終了処理です.
//-------------------------------------------------------------------------------------------------
void App::OnTerm()
{
    // テクスチャを解放.
    m_Texture.Release();

    // レンダーステートの終了処理.
    asdx::RenderState::GetInstance().Term();
}

//-------------------------------------------------------------------------------------------------
//      フレーム遷移処理です.
//-------------------------------------------------------------------------------------------------
void App::OnFrameMove(asdx::FrameEventArgs& param)
{
    ASDX_UNUSED_VAR( param );
}

//-------------------------------------------------------------------------------------------------
//      フレーム描画処理です.
//-------------------------------------------------------------------------------------------------
void App::OnFrameRender(asdx::FrameEventArgs& param)
{
    ASDX_UNUSED_VAR( param );

    {
        ID3D11RenderTargetView* pRTV = nullptr;
        ID3D11DepthStencilView* pDSV = nullptr;
        pRTV = m_ColorTarget2D.GetTargetView();
        pDSV = m_DepthTarget2D.GetTargetView();

        if ( pRTV == nullptr || pDSV == nullptr )
        { return; }

        m_pDeviceContext->OMSetRenderTargets( 1, &pRTV, pDSV );
        m_pDeviceContext->RSSetViewports( 1, &m_Viewport );

        m_pDeviceContext->ClearRenderTargetView( pRTV, m_ClearColor );
        m_pDeviceContext->ClearDepthStencilView( pDSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0 );

        m_Sprite.SetTexture( m_Texture.GetSRV(), asdx::RenderState::GetInstance().GetSmp( asdx::SamplerType::LinearClamp ) );
        m_Sprite.Begin( m_pDeviceContext.GetPtr(), asdx::Sprite::SHADER_TYPE_TEXTURE2D );
        m_Sprite.Draw( m_Width / 2 - 150, m_Height / 2 - 150, 300, 300, asdx::Vector2( 0.0f, 1.0f ), asdx::Vector2( 1.0f, 0.0f ) );
        m_Sprite.End( m_pDeviceContext.GetPtr() );
    }

    Present( 0 );
}

//-------------------------------------------------------------------------------------------------
//      リサイズ処理です.
//-------------------------------------------------------------------------------------------------
void App::OnResize(const asdx::ResizeEventArgs& param)
{
    m_Sprite.SetScreenSize( f32(param.Width), f32(param.Height) );
}

//-------------------------------------------------------------------------------------------------
//      キーイベントの処理です.
//-------------------------------------------------------------------------------------------------
void App::OnKey(const asdx::KeyEventArgs& param)
{
}

//-------------------------------------------------------------------------------------------------
//      マウスイベントの処理です.
//-------------------------------------------------------------------------------------------------
void App::OnMouse(const asdx::MouseEventArgs& param)
{
}