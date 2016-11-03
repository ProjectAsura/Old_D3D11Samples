//--------------------------------------------------------------------------------------------
// File : ShadowMgr.cpp
// Desc : Shadow Map Manager Module.
// Copyright(c) Project Asura. All right reserved.
//---------------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------------------------
#include <ShadowMgr.h>
#include <asdxLog.h>
#include <asdxShader.h>


namespace /* anonymous */ {

//---------------------------------------------------------------------------------------------
//      頂点シェーダを生成します.
//---------------------------------------------------------------------------------------------
bool CreateVS
(
    ID3D11Device*        pDevice,
    const wchar_t*       filename,
    ID3D11VertexShader** ppVS,
    ID3DBlob**           ppBlob,
    const char*          entryPoint = "VSFunc",
    asdx::ShaderHelper::SHADER_MODEL_TYPE model = asdx::ShaderHelper::VS_4_0
)
{
    ID3DBlob* pBlob = nullptr;

    HRESULT hr = S_OK;

    // 頂点シェーダをコンパイル.
    hr = asdx::ShaderHelper::CompileShaderFromFile( 
        filename,
        entryPoint,
        model,
        ppBlob );

    // エラーチェック.
    if ( FAILED( hr ) )
    {
        ASDX_RELEASE( (*ppBlob) );
        ELOG( "Error : CompileShaderFromFile() Failed." );
        return false;
    }

    // 頂点シェーダを生成.
    hr = pDevice->CreateVertexShader(
        (*ppBlob)->GetBufferPointer(),
        (*ppBlob)->GetBufferSize(),
        nullptr,
        ppVS );

    // エラーチェック.
    if ( FAILED( hr ) )
    {
        ELOG( "Error : ID3D11Device::CreateVertexShader() Failed." );
        return false;
    }

    return true;
}

//---------------------------------------------------------------------------------------------
//      ピクセルシェーダを生成します.
//---------------------------------------------------------------------------------------------
bool CreatePS
(
    ID3D11Device*       pDevice,
    const wchar_t*      filename,
    ID3D11PixelShader** ppPS,
    const char*         entryPoint = "PSFunc",
    asdx::ShaderHelper::SHADER_MODEL_TYPE model = asdx::ShaderHelper::PS_4_0
)
{
    ID3DBlob* pBlob = nullptr;

    HRESULT hr = S_OK;

    // ピクセルシェーダをコンパイル.
    hr = asdx::ShaderHelper::CompileShaderFromFile( 
        filename,
        entryPoint,
        model,
        &pBlob );

    // エラーチェック.
    if ( FAILED( hr ) )
    {
        ASDX_RELEASE( pBlob );
        ELOG( "Error : CompileShaderFromFile() Failed." );
        return false;
    }

    // ピクセルシェーダを生成.
    hr = pDevice->CreatePixelShader(
        pBlob->GetBufferPointer(),
        pBlob->GetBufferSize(),
        nullptr,
        ppPS );

    ASDX_RELEASE( pBlob );

    // エラーチェック.
    if ( FAILED( hr ) )
    {
        ELOG( "Error : ID3D11Device::CreateVertexShader() Failed." );
        return false;
    }

    return true;
}


///////////////////////////////////////////////////////////////////////////////////////////////
// CBGenShadow structure
///////////////////////////////////////////////////////////////////////////////////////////////
ASDX_ALIGN(16)
struct CBGenShadow
{
    asdx::Matrix        World;
    asdx::Matrix        View;
    asdx::Matrix        Proj;
};

} // namespace /* anonymous */


namespace asdx {

///////////////////////////////////////////////////////////////////////////////////////////////
// ShadowMgr class
///////////////////////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------------------------
//      コンストラクタです.
//---------------------------------------------------------------------------------------------
ShadowMgr::ShadowMgr()
: m_RT          ()
, m_DST         ()
, m_CBGenShadow ()
, m_pVSGenShadow( nullptr )
, m_pPSGenShadow( nullptr )
, m_pDSS        ( nullptr )
, m_pSmp        ( nullptr )
, m_LightDir    ( 0.0f, -1.0f, 0.0f )
, m_CameraPos   ()
, m_CameraDir   ()
, m_CameraUp    ()
, m_CameraNear  ()
, m_CameraFar   ()
, m_CameraFov   ()
, m_CasterBox   ()
{
    m_World.Identity();
    m_CameraView.Identity();
    m_CameraProj.Identity();
    m_LightView.Identity();
    m_LightProj.Identity();
}

//---------------------------------------------------------------------------------------------
//      デストラクタです.
//---------------------------------------------------------------------------------------------
ShadowMgr::~ShadowMgr()
{ Term(); }

//---------------------------------------------------------------------------------------------
//      初期化処理です.
//---------------------------------------------------------------------------------------------
bool ShadowMgr::Init( ID3D11Device* pDevice, const u32 width, const u32 height )
{

    {
        asdx::RenderTarget2D::Description desc;
        desc.Width              = width;
        desc.Height             = height;
        desc.ArraySize          = 1;
        desc.Format             = DXGI_FORMAT_R8G8B8A8_UNORM;
        desc.MipLevels          = 1;
        desc.SampleDesc.Count   = 1;
        desc.SampleDesc.Quality = 0;
        
        if ( !m_RT.Create( pDevice, desc ) )
        {
            ELOG( "Error : RenderTarget2D::Create() Failed." );
            return false;
        }
    }

    {
        asdx::DepthStencilTarget::Description desc;
        desc.Width              = width;
        desc.Height             = height;
        desc.ArraySize          = 1;
        desc.Format             = DXGI_FORMAT_D16_UNORM;
        desc.MipLevels          = 1;
        desc.SampleDesc.Count   = 1;
        desc.SampleDesc.Quality = 0;

        if ( !m_DST.Create( pDevice, desc ) )
        {
            ELOG( "Error : DepthStencilTarget::Create() Failed." );
            return false;
        }
    }

    ID3DBlob* pBlob = nullptr;
    if ( !CreateVS( pDevice, L"../res/shader/ShadowVS.hlsl", &m_pVSGenShadow, &pBlob ) )
    {
        ASDX_RELEASE( pBlob );
        ELOG( "Error : CreateVS() Failed." );
        return false;
    }
    ASDX_RELEASE( pBlob );

#if 0
    //if ( !CreatePS( pDevice, L"../res/shader/ShadowPS.hlsl", &m_pPSGenShadow ) )
    //{
    //    ELOG( "Error : CreatePS() Failed" );
    //    return false;
    //}
#endif

    if ( !m_CBGenShadow.Create( pDevice, sizeof(CBGenShadow) ) )
    {
        ELOG( "Error : ConstantBuffer::Create() Failed." );
        return false;
    }

    // サンプラーステートの生成.
    {
        D3D11_SAMPLER_DESC desc;
        ZeroMemory( &desc, sizeof( desc ) );
        desc.AddressU       = D3D11_TEXTURE_ADDRESS_BORDER;
        desc.AddressV       = D3D11_TEXTURE_ADDRESS_BORDER;
        desc.AddressW       = D3D11_TEXTURE_ADDRESS_BORDER;
        desc.BorderColor[0] = 1.0f;
        desc.BorderColor[1] = 1.0f;
        desc.BorderColor[2] = 1.0f;
        desc.BorderColor[3] = 1.0f;
        desc.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;
        desc.Filter         = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
        desc.MaxAnisotropy  = 1;
        desc.MipLODBias     = 0;
        desc.MinLOD         = -FLT_MAX;
        desc.MaxLOD         = +FLT_MAX;

        // サンプラーステートを生成.
        HRESULT hr = pDevice->CreateSamplerState( &desc, &m_pSmp );
        if ( FAILED( hr ) )
        {
            ELOG( "Error : ID3D11Device::CreateSamplerState() Failed." );
            return false;
        }
    }

    // ビューポートの設定.
    {
        m_Viewport.Width    = f32(width);
        m_Viewport.Height   = f32(height);
        m_Viewport.TopLeftX = 0;
        m_Viewport.TopLeftY = 0;
        m_Viewport.MinDepth = 0.0f;
        m_Viewport.MaxDepth = 1.0f;
    }

    return true;
}

//---------------------------------------------------------------------------------------------
//      終了処理です.
//---------------------------------------------------------------------------------------------
void ShadowMgr::Term()
{
    m_RT.Release();
    m_DST.Release();
    m_CBGenShadow.Release();
    ASDX_RELEASE( m_pVSGenShadow );
    ASDX_RELEASE( m_pPSGenShadow );
    ASDX_RELEASE( m_pDSS );
    ASDX_RELEASE( m_pSmp );
}

//---------------------------------------------------------------------------------------------
//      シャドウ生成パスを開始します.
//---------------------------------------------------------------------------------------------
void ShadowMgr::Begin( ID3D11DeviceContext* pDeviceContext )
{
#if 0 
    //// レンダーターゲットに深度値を書き込む場合はこちらを使用
    //ID3D11RenderTargetView* pRTV = m_RT.GetRTV();
    //ID3D11DepthStencilView* pDSV = m_DST.GetDSV();

    //f32 colors[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
    //pDeviceContext->OMSetRenderTargets( 1, &pRTV, pDSV );
    //pDeviceContext->ClearRenderTargetView( pRTV, colors );
    //pDeviceContext->ClearDepthStencilView( pDSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0 );

    //pDeviceContext->VSSetShader( m_pVSGenShadow, nullptr, 0 );
    //pDeviceContext->GSSetShader( nullptr, nullptr, 0 );
    //pDeviceContext->PSSetShader( m_pPSGenShadow, nullptr, 0 );
    //pDeviceContext->DSSetShader( nullptr, nullptr, 0 );
    //pDeviceContext->HSSetShader( nullptr, nullptr, 0 );
#else
    // 深度ステンシルターゲットのみに深度値を書き込む場合はこちらを使用.
    ID3D11DepthStencilView* pDSV = m_DST.GetDSV();
    pDeviceContext->OMSetRenderTargets( 0, nullptr, pDSV );
    pDeviceContext->ClearDepthStencilView( pDSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0 );

    pDeviceContext->VSSetShader( m_pVSGenShadow, nullptr, 0 );
    pDeviceContext->GSSetShader( nullptr, nullptr, 0 );
    pDeviceContext->PSSetShader( nullptr, nullptr, 0 );
    pDeviceContext->DSSetShader( nullptr, nullptr, 0 );
    pDeviceContext->HSSetShader( nullptr, nullptr, 0 );
#endif

    pDeviceContext->RSSetViewports( 1, &m_Viewport );
}

//---------------------------------------------------------------------------------------------
//      シャドウ生成パスを終了します.
//---------------------------------------------------------------------------------------------
void ShadowMgr::End( ID3D11DeviceContext* pDeviceContext )
{
    pDeviceContext->VSSetShader( nullptr, nullptr, 0 );
    pDeviceContext->GSSetShader( nullptr, nullptr, 0 );
    pDeviceContext->PSSetShader( nullptr, nullptr, 0 );
    pDeviceContext->DSSetShader( nullptr, nullptr, 0 );
    pDeviceContext->HSSetShader( nullptr, nullptr, 0 );
}

//---------------------------------------------------------------------------------------------
//      シャドウ生成パスの定数バッファを更新します.
//---------------------------------------------------------------------------------------------
void ShadowMgr::UpdateCB( ID3D11DeviceContext* pDeviceContext )
{
    CBGenShadow param;
    param.World = m_World;
    param.View  = m_LightView;
    param.Proj  = m_LightProj;

    ID3D11Buffer* pCB = m_CBGenShadow.GetBuffer();
    pDeviceContext->UpdateSubresource( pCB, 0, nullptr, &param, 0, 0 );
    pDeviceContext->VSSetConstantBuffers( 0, 1, &pCB );
}

//---------------------------------------------------------------------------------------------
//      シャドウマップ行列を求めます.
//---------------------------------------------------------------------------------------------
void ShadowMgr::ComputeMatrix()
{
    ComputeMatrixUSM();
}

//---------------------------------------------------------------------------------------------
//      単位キューブクリッピング行列を求めます.
//---------------------------------------------------------------------------------------------
asdx::Matrix ShadowMgr::UnitCubeClip
(
    const asdx::Vector3& mini,
    const asdx::Vector3& maxi
)
{
    // 単位キューブクリップ行列を求める.
    asdx::Matrix clip;
    clip._11 = 2.0f / ( maxi.x - mini.x );
    clip._12 = 0.0f;
    clip._13 = 0.0f;
    clip._14 = 0.0f;

    clip._21 = 0.0f;
    clip._22 = 2.0f / ( maxi.y - mini.y );
    clip._23 = 0.0f;
    clip._24 = 0.0f;

    clip._31 = 0.0f;
    clip._32 = 0.0f;
    clip._33 = 1.0f / ( maxi.z - mini.z );
    clip._34 = 0.0f;

    clip._41 = -( maxi.x + mini.x ) / ( maxi.x - mini.x );
    clip._42 = -( maxi.y + mini.y ) / ( maxi.y - mini.y );
    clip._43 = - mini.z / ( maxi.z - mini.z );
    clip._44 = 1.0f;

    return clip;
}

//---------------------------------------------------------------------------------------------
//      均一シャドウマップ行列を求めます.
//---------------------------------------------------------------------------------------------
void ShadowMgr::ComputeMatrixUSM()
{
    asdx::Vector3x8 convexHull;
    m_CasterBox.GetCorners( convexHull );

    for( u32 i=0; i<convexHull.GetSize(); ++i )
    {
        Vector3 val = Vector3::Transform( convexHull[i], m_World );
        convexHull.SetAt( i, val );
    }

    //----------------------------------------
    //   ライトのビュー射影行列を求める.
    //----------------------------------------
    {
        // ライトのビュー行列を生成.
        asdx::Matrix lightView = asdx::Matrix::CreateLookTo(
            asdx::Vector3( 0.0f, 0.0f, 0.0f ),
            m_LightBasis.w,
            m_LightBasis.v );

        // ライトビュー空間でのAABBを求める.
        asdx::Vector3 point = asdx::Vector3::TransformCoord( convexHull[0], lightView );
        asdx::Vector3 mini = point;
        asdx::Vector3 maxi = point;
        {
            for( u32 i=1; i<convexHull.GetSize(); ++i )
            {
                point = asdx::Vector3::TransformCoord( convexHull[i], lightView );
                mini  = asdx::Vector3::Min( mini, point );
                maxi  = asdx::Vector3::Max( maxi, point );
            }
        }

        // ライトビュー空間での中心を求める.
        asdx::Vector3 center = ( mini + maxi ) * 0.5f;

        // ニアクリップ平面とファークリップ平面の距離を求める.
        f32 nearClip  = 1.0f;
        f32 farClip   = fabs( maxi.z - mini.z ) * 1.001f + nearClip;

        // 後退量を求める.
        f32 slideBack = fabs( center.z - mini.z ) + nearClip;

        // 正しいライト位置を求める.
        asdx::Vector3 lightPos = center - ( m_LightBasis.w * slideBack );

        // ライトのビュー行列を算出し直す.
        m_LightView = asdx::Matrix::CreateLookTo(
            lightPos,
            m_LightBasis.w,
            m_LightBasis.v);

        // 求め直したライトのビュー行列を使ってAABBを求める.
        point = asdx::Vector3::TransformCoord( convexHull[0], m_LightView );
        mini  = point;
        maxi  = point;
        {
            for( u32 i=1; i<convexHull.GetSize(); ++i )
            {
                point = asdx::Vector3::TransformCoord( convexHull[i], m_LightView );
                mini  = asdx::Vector3::Min( mini, point );
                maxi  = asdx::Vector3::Max( maxi, point );
            }
        }

        // サイズを求める.
        f32 size = ( maxi - mini ).Length();

        // ライトの射影行列/
        m_LightProj = asdx::Matrix::CreateOrthographic(
            size,
            size,
            nearClip,
            farClip );
    }

    //---------------------------
    // 単位キューブクリッピング.
    //---------------------------
    {
        // ライトのビュー射影行列を求める.
        asdx::Matrix lightViewProj = m_LightView * m_LightProj;

        // ライトのビュー射影空間でのAABBを求める.
        asdx::Vector3 point = asdx::Vector3::TransformCoord( convexHull[0], lightViewProj );
        asdx::Vector3 mini = point;
        asdx::Vector3 maxi = point;
        {
            for( u32 i=1; i<convexHull.GetSize(); ++i )
            {
                point = asdx::Vector3::TransformCoord( convexHull[i], lightViewProj );
                mini  = asdx::Vector3::Min( mini, point );
                maxi  = asdx::Vector3::Max( maxi, point );
            }
        }

        // 極端にゆがまないように制限を掛ける.
        mini.x = asdx::Clamp( mini.x, -1.0f, 1.0f );
        maxi.x = asdx::Clamp( maxi.x, -1.0f, 1.0f );

        mini.y = asdx::Clamp( mini.y, -1.0f, 1.0f );
        maxi.y = asdx::Clamp( maxi.y, -1.0f, 1.0f );

        // クリッピング行列を掛ける.
        m_LightProj = m_LightProj * UnitCubeClip( mini, maxi );
    }
}

//---------------------------------------------------------------------------------------------
//      ライトの方向ベクトルを設定します.
//---------------------------------------------------------------------------------------------
void ShadowMgr::SetLightDir( const asdx::Vector3& lightDir )
{
    m_LightDir = lightDir;
    m_LightDir.Normalize();
    m_LightBasis.InitFromW( m_LightDir );
}

//---------------------------------------------------------------------------------------------
//      カメラ位置を設定します.
//---------------------------------------------------------------------------------------------
void ShadowMgr::SetCameraPos( const asdx::Vector3& pos )
{ m_CameraPos = pos; }

//---------------------------------------------------------------------------------------------
//      カメラの方向ベクトルを設定します.
//---------------------------------------------------------------------------------------------
void ShadowMgr::SetCameraDir( const asdx::Vector3& dir )
{ m_CameraDir = dir; }

//---------------------------------------------------------------------------------------------
//      カメラの上向きベクトルを設定します.
//---------------------------------------------------------------------------------------------
void ShadowMgr::SetCameraUp ( const asdx::Vector3& upward )
{ m_CameraUp = upward; }

//---------------------------------------------------------------------------------------------
//      カメラのニア平面を設定します.
//---------------------------------------------------------------------------------------------
void ShadowMgr::SetCameraNear( const f32 nearClip )
{ m_CameraNear = nearClip; }

//---------------------------------------------------------------------------------------------
//      カメラのファー平面を設定します.
//---------------------------------------------------------------------------------------------
void ShadowMgr::SetCameraFar ( const f32 farClip ) 
{ m_CameraFar = farClip; }

//---------------------------------------------------------------------------------------------
//      カメラの垂直画角を設定します.
//---------------------------------------------------------------------------------------------
void ShadowMgr::SetCameraFov ( const f32 fovy )
{ m_CameraFov = fovy; }

//---------------------------------------------------------------------------------------------
//      ワールド行列を設定します.
//---------------------------------------------------------------------------------------------
void ShadowMgr::SetWorld( const asdx::Matrix& world ) 
{ m_World = world; }

//---------------------------------------------------------------------------------------------
//      カメラのビュー行列を設定します.
//---------------------------------------------------------------------------------------------
void ShadowMgr::SetCameraView( const asdx::Matrix& view )
{ m_CameraView = view; }

//---------------------------------------------------------------------------------------------
//      カメラの射影行列を設定します.
//---------------------------------------------------------------------------------------------
void ShadowMgr::SetCameraProj( const asdx::Matrix& proj )
{ m_CameraProj = proj; }

//---------------------------------------------------------------------------------------------
//      キャスターとなる範囲のバウンディングボックスを設定します.
//---------------------------------------------------------------------------------------------
void ShadowMgr::SetCasterBox( const asdx::BoundingBox& box )
{ m_CasterBox = box; }


} // namespace asdx
