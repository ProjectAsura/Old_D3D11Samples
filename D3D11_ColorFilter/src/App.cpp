//-----------------------------------------------------------------------------
// File : App.cpp
// Desc : Application.
// Copyright(c) Project Asura. All right reserved.
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <App.h>
#include <asdxLogger.h>
#include <asdxRenderState.h>
#include <asdxMisc.h>

#include "../res/shader/Compiled/ColorFilterCS.inc"


namespace {

///////////////////////////////////////////////////////////////////////////////
// CbColorFilter structure
///////////////////////////////////////////////////////////////////////////////
struct alignas(16) CbColorFilter
{
    uint32_t        ThreadX;
    uint32_t        ThreadY;
    uint32_t        Padding[2];
    asdx::Matrix    ColorMatrix;
};

asdx::Matrix CreateBrightnessMatrix(float brightness)
{
    return asdx::Matrix::CreateScale(brightness);
}

asdx::Matrix CreateSaturationMatrix(float r, float g, float b)
{
    // https://docs.microsoft.com/ja-jp/windows/win32/direct2d/saturation
    return asdx::Matrix(
        0.213f + 0.787f * r,
        0.213f - 0.213f * r,
        0.213f - 0.213f * r,
        0.0f,

        0.715f - 0.715f * g,
        0.715f + 0.285f * g,
        0.715f - 0.715f * g,
        0.0f,

        0.072f - 0.072f * b,
        0.072f - 0.072f * b,
        0.072f + 0.928f * b,
        0.0f,

        0.0f, 0.0f, 0.0f, 1.0f);
}

asdx::Matrix CreateSaturationMatrix(float saturation)
{
    return CreateSaturationMatrix(saturation, saturation, saturation);
}

asdx::Matrix CreateContrastMatrix(float contrast)
{
    const auto t = (1.0f - contrast) * 0.5f;
    return asdx::Matrix(
        contrast, 0.0f, 0.0f, 0.0f,
        0.0f, contrast, 0.0f, 0.0f,
        0.0f, 0.0f, contrast, 0.0f,
        t, t, t, 1.0f);
}

asdx::Matrix CreateHueMatrix(float hue)
{
    // https://docs.microsoft.com/ja-jp/windows/win32/direct2d/hue-rotate
    auto rad = asdx::ToRadian(hue);
    auto u = cosf(rad);
    auto w = sinf(rad);

    return asdx::Matrix(
        0.213f + 0.787f * u - 0.213f * w,
        0.213f - 0.213f * u + 0.143f * w,
        0.213f - 0.213f * u - 0.787f * w,
        0.0f,

        0.715f - 0.715f * u - 0.715f * w,
        0.715f + 0.285f * u + 0.140f * w,
        0.715f - 0.715f * u - 0.283f * w,
        0.0f,

        0.072f - 0.072f * u + 0.928f * w,
        0.072f - 0.072f * u - 0.283f * w,
        0.072f + 0.928f * u + 0.072f * w,
        0.0f,

        0.0f,
        0.0f,
        0.0f,
        1.0f);
}

asdx::Matrix CreateSepiaMatrix(float tone)
{
    const asdx::Vector3 W(0.298912f, 0.586611f, 0.114478f);
    const asdx::Vector3 Sepia(0.941f, 0.784f, 0.569f);

    return asdx::Matrix(
        tone * W.x * Sepia.x + (1.0f - tone),
        tone * W.x * Sepia.y,
        tone * W.x * Sepia.z,
        0.0f,

        tone * W.y * Sepia.x,
        tone * W.y * Sepia.y + (1.0f - tone),
        tone * W.y * Sepia.z,
        0.0f,

        tone * W.z * Sepia.x,
        tone * W.z * Sepia.y,
        tone * W.z * Sepia.z + (1.0f - tone),
        0.0f,

        0.0f, 0.0f, 0.0f, 1.0f);
}

asdx::Matrix CreateGrayScaleMatrix(float tone)
{
    const asdx::Vector3 GrayScale(0.22015f, 0.706655f, 0.071330f);
    return asdx::Matrix(
        tone * GrayScale.x + (1.0f - tone),
        tone * GrayScale.x,
        tone * GrayScale.x,
        0.0f,

        tone * GrayScale.y,
        tone * GrayScale.y + (1.0f - tone),
        tone * GrayScale.y,
        0.0f,

        tone * GrayScale.z,
        tone * GrayScale.z,
        tone * GrayScale.z + (1.0f - tone),
        0.0f,

        0.0f, 0.0f, 0.0f, 1.0f);
}

asdx::Matrix CreateNegaposiMatrix()
{
    return asdx::Matrix(
        -1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, -1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, -1.0f, 0.0f,
        1.0f, 1.0f, 1.0f, 1.0f);
}

asdx::Matrix CreateSwapRandG()
{
    return asdx::Matrix(
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f);
}

} // namespace 


///////////////////////////////////////////////////////////////////////////////
// App class
///////////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------
//      コンストラクタです.
//-----------------------------------------------------------------------------
App::App()
: asdx::Application(L"ColorFilter", 960, 540, NULL, NULL, NULL)
{
}

//-----------------------------------------------------------------------------
//      デストラクタです.
//-----------------------------------------------------------------------------
App::~App()
{
}

//-----------------------------------------------------------------------------
//      初期化時の処理です.
//-----------------------------------------------------------------------------
bool App::OnInit()
{
    // スプライトの初期化に失敗しました.
    if ( !m_Sprite.Init( m_pDevice.GetPtr(), float(m_Width), float(m_Height) ) )
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

    // テクスチャ初期化.
    {
        std::string path;
        if (!asdx::SearchFilePathA("res/texture/sample.tga", path))
        {
            ELOG("Error : Not Found Path.");
            return false;
        }

        asdx::ResTexture res;
        if (!res.LoadFromFileA(path.c_str()))
        {
            ELOG("Error : Texture Load Failed. path = %s", path.c_str());
            return false;
        }


        if (!m_Texture.Create(m_pDevice.GetPtr(), m_pDeviceContext.GetPtr(), res))
        {
            ELOG("Error : Texture Initialize Failed.");
            return false;
        }
    }

    // コンピュートシェーダ初期化.
    if (!m_CS.Init(m_pDevice.GetPtr(), ColorFilterCS, sizeof(ColorFilterCS)))
    {
        ELOG("Error : ComputeShader::Initialize() Failed.");
        return false;
    }

    // スレッドグループサイズ取得.
    m_CS.GetReflection()->GetThreadGroupSize(
        &m_ThreadCountX,
        &m_ThreadCountY,
        &m_ThreadCountZ);
    assert(m_ThreadCountX == m_ThreadCountY);

    // コンピュート用ターゲット生成.
    {
        D3D11_TEXTURE2D_DESC desc = {};
        m_Texture.GetTexture()->GetDesc(&desc);

        desc.MipLevels      = 1;
        desc.Usage          = D3D11_USAGE_DEFAULT;
        desc.BindFlags      = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
        desc.CPUAccessFlags = 0;
        desc.MiscFlags      = 0;

        auto hr = m_pDevice->CreateTexture2D(&desc, nullptr, m_ComputeTex.GetAddress());
        if (FAILED(hr))
        {
            ELOG("Error : ID3D11Device::CreateTexture2D() Failed. errcode = 0x%x", hr);
            return false;
        }

        D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
        uavDesc.ViewDimension       = D3D11_UAV_DIMENSION_TEXTURE2D;
        uavDesc.Format              = DXGI_FORMAT_R8G8B8A8_UNORM;
        uavDesc.Texture2D.MipSlice  = 0;

        hr = m_pDevice->CreateUnorderedAccessView(m_ComputeTex.GetPtr(), &uavDesc, m_ComputeUAV.GetAddress());
        if (FAILED(hr))
        {
            ELOG("Error : ID3D11Device::CreateUnorderedAccessView() Failed. errcode = 0x%x", hr);
            return false;
        }

        D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
        srvDesc.ViewDimension               = D3D11_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Format                      = DXGI_FORMAT_R8G8B8A8_UNORM;
        srvDesc.Texture2D.MipLevels         = 1;
        srvDesc.Texture2D.MostDetailedMip   = 0;

        hr = m_pDevice->CreateShaderResourceView(m_ComputeTex.GetPtr(), &srvDesc, m_ComputeSRV.GetAddress());
        if (FAILED(hr))
        {
            ELOG("Error : ID3D11Device::CreateShaderResourceView() Failed. errcode = 0x%x", hr);
            return false;
        }

        m_TextureWidth  = desc.Width;
        m_TextureHeight = desc.Height;
    }

    // カラーフィルタ用定数バッファ.
    {
        if (!m_CB.Init(m_pDevice.GetPtr(), sizeof(CbColorFilter)))
        {
            ELOG("Error : ConstantBuffer::Init() Failed.");
            return false;
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
//      終了時の処理です.
//-----------------------------------------------------------------------------
void App::OnTerm()
{
    // テクスチャを解放.
    m_Texture.Release();

    m_CS.Term();
    m_CB.Term();
    m_ComputeUAV.Reset();
    m_ComputeSRV.Reset();
    m_ComputeTex.Reset();

    // スプライト終了.
    m_Sprite.Term();

    // レンダーステートの終了処理.
    asdx::RenderState::GetInstance().Term();
}

//-----------------------------------------------------------------------------
//      描画時の処理です.
//-----------------------------------------------------------------------------
void App::OnFrameRender(asdx::FrameEventArgs& param)
{
    ID3D11RenderTargetView* pRTV = nullptr;
    ID3D11DepthStencilView* pDSV = nullptr;
    pRTV = m_ColorTarget2D.GetTargetView();
    pDSV = m_DepthTarget2D.GetTargetView();

    if ( pRTV == nullptr || pDSV == nullptr )
    { return; }

    // カラーフィルタ実行.
    {
        auto x = (m_TextureWidth  + m_ThreadCountX - 1) / m_ThreadCountX;
        auto y = (m_TextureHeight + m_ThreadCountY - 1) / m_ThreadCountY;

        auto pCB = m_CB.GetBuffer();
        CbColorFilter res = {};
        res.ThreadX = x;
        res.ThreadY = y;
        res.ColorMatrix = CreateSepiaMatrix(0.85f);

        m_pDeviceContext->UpdateSubresource(pCB, 0, nullptr, &res, 0, 0);

        auto pSRV = m_Texture.GetSRV();
        auto pUAV = m_ComputeUAV.GetPtr();
        m_CS.Bind(m_pDeviceContext.GetPtr());
        m_pDeviceContext->CSSetConstantBuffers(0, 1, &pCB);
        m_pDeviceContext->CSSetShaderResources(0, 1, &pSRV);
        m_pDeviceContext->CSSetUnorderedAccessViews(0, 1, &pUAV, nullptr);
        m_pDeviceContext->Dispatch(x, y, 1);

        ID3D11ShaderResourceView* pNullSRV[1] = {};
        ID3D11UnorderedAccessView* pNullUAV[1] = {};
        m_pDeviceContext->CSSetShaderResources(0, 1, pNullSRV);
        m_pDeviceContext->CSSetUnorderedAccessViews(0, 1, pNullUAV, nullptr);
        m_CS.UnBind(m_pDeviceContext.GetPtr());
    }

    // バックバッファに描画.
    {
        m_pDeviceContext->OMSetRenderTargets( 1, &pRTV, pDSV );
        m_pDeviceContext->RSSetViewports( 1, &m_Viewport );
        m_pDeviceContext->RSSetScissorRects( 1, &m_ScissorRect );

        m_pDeviceContext->ClearRenderTargetView( pRTV, m_ClearColor );
        m_pDeviceContext->ClearDepthStencilView( pDSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0 );

        m_Sprite.SetTexture( m_ComputeSRV.GetPtr(), asdx::RenderState::GetInstance().GetSmp( asdx::SamplerType::LinearClamp ) );
        m_Sprite.Begin( m_pDeviceContext.GetPtr(), asdx::Sprite::SHADER_TYPE_TEXTURE2D );
        m_Sprite.Draw( m_Width / 2 - 150, m_Height / 2 - 150, 300, 300 );
        m_Sprite.End( m_pDeviceContext.GetPtr() );
    }

    Present( 0 );
}

//-----------------------------------------------------------------------------
//      リサイズ時の処理です.
//-----------------------------------------------------------------------------
void App::OnResize(const asdx::ResizeEventArgs& param)
{
    m_Sprite.SetScreenSize( float(param.Width), float(param.Height) );
}

