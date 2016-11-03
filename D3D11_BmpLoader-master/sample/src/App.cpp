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
#include <asdxResBMP.h>
#include <asdxRenderState.h>
#include <App.h>
#include <cassert>


namespace /* anonymous */ {

//-------------------------------------------------------------------------------------------------
//! @brief      BMPデータからリソーステクスチャを生成します.
//!
//! @param[in]      value       BMPデータ.
//! @return     TGAデータから生成したリソーステクスチャを返却します.
//-------------------------------------------------------------------------------------------------
asdx::ResTexture CreateFromBmp( const asdx::ResBMP& value )
{
    asdx::ResTexture result;
    result.Width        = value.GetWidth();
    result.Height       = value.GetHeight();
    result.Depth        = 0;
    result.Format       = ( value.GetFormat() == asdx::ResBMP::Format_RGBA_SRGB 
                         || value.GetFormat() == asdx::ResBMP::Format_RGB_SRGB ) ? DXGI_FORMAT_R8G8B8A8_UNORM_SRGB : DXGI_FORMAT_R8G8B8A8_UNORM;;
    result.MipMapCount  = 1;
    result.SurfaceCount = 1;

    auto pSubResource = new asdx::SubResource();
    pSubResource->Width      = value.GetWidth();
    pSubResource->Height     = value.GetHeight();
    pSubResource->Pitch      = value.GetWidth() * 4;
    pSubResource->SlicePitch = pSubResource->Pitch * value.GetHeight();
    pSubResource->pPixels    = new u8 [ pSubResource->SlicePitch ];
    assert( pSubResource->pPixels != nullptr );
    result.pResources = pSubResource;

    switch( value.GetFormat() )
    {
    case asdx::ResBMP::Format_RGB:
    case asdx::ResBMP::Format_RGB_SRGB:
        {
            auto pixels = value.GetPixels();
            auto size   = value.GetWidth() * value.GetHeight();
            for( u32 i=0; i<size; ++i )
            {
                pSubResource->pPixels[ i * 4 + 0 ] = pixels[ i * 3 + 0 ];
                pSubResource->pPixels[ i * 4 + 1 ] = pixels[ i * 3 + 1 ];
                pSubResource->pPixels[ i * 4 + 2 ] = pixels[ i * 3 + 2 ];
                pSubResource->pPixels[ i * 4 + 3 ] = 255;
            }
        }
        break;

    case asdx::ResBMP::Format_RGBA:
    case asdx::ResBMP::Format_RGBA_SRGB:
        {
            memcpy(pSubResource->pPixels, value.GetPixels(), pSubResource->SlicePitch );
        }
        break;
    }

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
    const char16* loadPath = L"res/sample24Bit.bmp";
    //const char16* loadPath = L"res/sample8Bit.bmp";
    //const char16* loadPath = L"res/sample4Bit.bmp";
    //const char16* loadPath = L"res/sample1Bit.bmp";
    //const char16* loadPath = L"res/sampleOS2.bmp";
    //const char16* loadPath = L"res/sample8bitRLE.bmp";
    //const char16* loadPath = L"res/sample4bitRLE.bmp";


    std::wstring path;
    if ( !asdx::SearchFilePath( loadPath, path ) )
    {
        ELOG( "Error : File Not Found." );
        return false;
    }

    // BMPをロード.
    asdx::ResBMP bmp;
    if ( !bmp.Load( path.c_str() ) )
    {
        ELOG( "Error : File Load Failed. path = %s", path.c_str() );
        return false;
    }

    // リソーステクスチャにコンバート.
    asdx::ResTexture res = CreateFromBmp( bmp );

    // BMPを解放.
    bmp.Release();

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
        m_Sprite.Draw( m_Width / 2 - 150, m_Height / 2 - 150, 300, 300 );
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