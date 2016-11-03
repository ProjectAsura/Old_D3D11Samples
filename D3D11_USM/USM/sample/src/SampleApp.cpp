//-----------------------------------------------------------------------------------
// File : sampleApp.h
// Desc : Sample Application Module.
// Copyright(c) Project Asura. All right reserved.
//-----------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------------
#include <sampleApp.h>
#include <asdxShader.h>
#include <asdxLog.h>
#include <vector>


// テクスチャバイアス.
static const asdx::Matrix SHADOW_BIAS = asdx::Matrix(
    0.5f,  0.0f, 0.0f, 0.0f,
    0.0f, -0.5f, 0.0f, 0.0f,
    0.0f,  0.0f, 1.0f, 0.0f,
    0.5f,  0.5f, 0.0f, 1.0f );

/////////////////////////////////////////////////////////////////////////////////////
// QuadParam structure
/////////////////////////////////////////////////////////////////////////////////////
ASDX_ALIGN(16)
struct QuadParam
{
    asdx::Matrix matrix;
};

//////////////////////////////////////////////////////////////////////////////////////
// CBForward structure
//////////////////////////////////////////////////////////////////////////////////////
ASDX_ALIGN(16)
struct CBForward
{
    asdx::Matrix        World;
    asdx::Matrix        View;
    asdx::Matrix        Proj;
    asdx::Vector3       CameraPos;
    f32                 dummy0;
    asdx::Vector3       LightDir;
    f32                 dummy1;
    asdx::Matrix        Shadow;
};

/////////////////////////////////////////////////////////////////////////////////////
// SampleApp class
/////////////////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------------
//      コンストラクタです.
//-----------------------------------------------------------------------------------
SampleApp::SampleApp()
: Application( "Uniform Shadow Map", 960, 540 )
, m_Quad    ()
, m_pQuadVS ( nullptr )
, m_pQuadPS ( nullptr )
, m_pQuadCB ( nullptr )
, m_pQuadSmp( nullptr )
, m_pVS     ( nullptr )
, m_pPS     ( nullptr )
, m_pCBMatrixForward( nullptr )
, m_Dosei   ()
, m_LightRotX( asdx::F_PIDIV4 )
, m_LightRotY( asdx::F_PIDIV2 )
, m_ShowTexture( true )
, m_CameraMode( 0 )
, m_SdwMgr()
{
    /* DO_NOTHING */
}

//-----------------------------------------------------------------------------------
//      デストラクタです.
//-----------------------------------------------------------------------------------
SampleApp::~SampleApp()
{
    /* DO_NOTHING */
}

//-----------------------------------------------------------------------------------
//      矩形の初期化処理です.
//-----------------------------------------------------------------------------------
bool SampleApp::InitQuad()
{
    HRESULT hr = S_OK;
    {
        ID3DBlob* pBlob;

        // 頂点シェーダをコンパイル.
        hr = asdx::ShaderHelper::CompileShaderFromFile( 
            L"../res/shader/QuadRenderer.hlsl",
            "VSFunc",
            asdx::ShaderHelper::VS_4_0,
            &pBlob );

        // エラーチェック.
        if ( FAILED( hr ) )
        {
            ELOG( "Error : CompileShaderFromFile() Failed." );
            return false;
        }

        // 頂点シェーダを生成.
        hr = m_pDevice->CreateVertexShader(
            pBlob->GetBufferPointer(),
            pBlob->GetBufferSize(),
            nullptr,
            &m_pQuadVS );

        // エラーチェック.
        if ( FAILED( hr ) )
        {
            ASDX_RELEASE( pBlob );
            ELOG( "Error : ID3D11Device::CreateVertexShader() Failed." );
            return false;
        }

        // 矩形ヘルパーの初期化.
        if ( !m_Quad.Init( m_pDevice, pBlob->GetBufferPointer(), pBlob->GetBufferSize() ) )
        {
            ASDX_RELEASE( pBlob );
            ELOG( "Error : QuadRenderer::Init() Failed." );
            return false;
        }

        // 解放.
        ASDX_RELEASE( pBlob );

        // ピクセルシェーダをコンパイル.
        hr = asdx::ShaderHelper::CompileShaderFromFile(
            L"../res/shader/QuadRenderer.hlsl",
            "PSFunc",
            asdx::ShaderHelper::PS_4_0,
            &pBlob );

        // ピクセルシェーダを生成.
        hr = m_pDevice->CreatePixelShader(
            pBlob->GetBufferPointer(),
            pBlob->GetBufferSize(),
            nullptr,
            &m_pQuadPS );

        // 解放.
        ASDX_RELEASE( pBlob );

        // エラーチェック.
        if ( FAILED( hr ) )
        {
            ELOG( "Error : ID3D11Device::CreatePixelShader() Failed." );
            return false;
        }
    }

    // 定数バッファの生成.
    {
        D3D11_BUFFER_DESC desc;
        ZeroMemory( &desc, sizeof(desc) );
        desc.Usage          = D3D11_USAGE_DEFAULT;
        desc.ByteWidth      = sizeof( QuadParam );
        desc.BindFlags      = D3D11_BIND_CONSTANT_BUFFER;
        desc.CPUAccessFlags = 0;

        // 定数バッファを生成.
        hr = m_pDevice->CreateBuffer( &desc, nullptr, &m_pQuadCB );
        if ( FAILED( hr ) )
        {
            ELOG( "Error : ID3D11Device::CreateBuffer()" );
            return false;
        }
    }

    // サンプラーステートを生成.
    {
        CD3D11_SAMPLER_DESC desc = CD3D11_SAMPLER_DESC( CD3D11_DEFAULT() );
        
        // サンプラーステートを生成.
        hr = m_pDevice->CreateSamplerState( &desc, &m_pQuadSmp );
        if ( FAILED( hr ) )
        {
            ELOG( "Error : ID3D11Device::CreateSamplerState() Failed." );
            return false;
        }
    }

    // 正常終了.
    return true;
}

//-----------------------------------------------------------------------------------
//      矩形の終了処理を行います.
//-----------------------------------------------------------------------------------
void SampleApp::TermQuad()
{
    m_Quad.Term();
    ASDX_RELEASE( m_pQuadVS );
    ASDX_RELEASE( m_pQuadPS );
    ASDX_RELEASE( m_pQuadCB );
    ASDX_RELEASE( m_pQuadSmp );
}

//-----------------------------------------------------------------------------------
//      シャドウステートの初期化を行います.
//-----------------------------------------------------------------------------------
bool SampleApp::InitShadowState()
{
    u32 mapSize = 2048;
    return m_SdwMgr.Init( m_pDevice, mapSize, mapSize );
}

//-----------------------------------------------------------------------------------
//      シャドウステートの終了処理です.
//-----------------------------------------------------------------------------------
void SampleApp::TermShadowState()
{
    m_SdwMgr.Term();
}

//-----------------------------------------------------------------------------------
//      フォワードレンダリングの初期化処理です.
//-----------------------------------------------------------------------------------
bool SampleApp::InitForward()
{
    HRESULT hr = S_OK;

    // メッシュの読み込み.
    {
        ID3DBlob* pVSBlob = nullptr;
        hr = asdx::ShaderHelper::CompileShaderFromFile(
            L"../res/shader/ForwardVS.hlsl",
            "VSFunc",
            asdx::ShaderHelper::VS_4_0,
            &pVSBlob );
        if ( FAILED( hr ) )
        {
            ELOG( "Error : CompileShaderFromFile() Failed." );
            return false;
        }

        asdx::ResMesh resMesh;
        if ( !resMesh.LoadFromFile( "../res/scene/scene.msh" ) )
        {
            ASDX_RELEASE( pVSBlob );
            ELOG( "Error : Mesh Load Failed." );
            return false;
        }

        // AABBを求めておく.
        if ( resMesh.GetVertexCount() >= 1 )
        {
            asdx::Vector3 mini = resMesh.GetVertex(0).Position;
            asdx::Vector3 maxi = resMesh.GetVertex(0).Position;

            for( u32 i=1; i<resMesh.GetVertexCount(); ++i )
            {
                mini = asdx::Vector3::Min( mini, resMesh.GetVertex(i).Position );
                maxi = asdx::Vector3::Max( maxi, resMesh.GetVertex(i).Position );
            }

            m_Box_Dosei = asdx::BoundingBox( mini, maxi );
        }

        if ( !m_Dosei.Init( 
            m_pDevice,
            resMesh,
            pVSBlob->GetBufferPointer(),
            pVSBlob->GetBufferSize(),
            "../res/scene/",
            "../res/dummy/" ) )
        {
            ELOG( "Error : Mesh Init Falied." );
            ASDX_RELEASE( pVSBlob );
            return false;
        }

        resMesh.Release();

        hr = m_pDevice->CreateVertexShader( pVSBlob->GetBufferPointer(),
            pVSBlob->GetBufferSize(),
            nullptr,
            &m_pVS );

        ASDX_RELEASE( pVSBlob );
        if ( FAILED( hr ) )
        {
            ELOG( "Error : ID3D11Device::CreateVertexShader() Failed." );
            return false;
        }
    }

    // ピクセルシェーダの生成.
    {
        ID3DBlob* pBlob;

        hr = asdx::ShaderHelper::CompileShaderFromFile(
            L"../res/shader/ForwardPS.hlsl",
            "PSFunc",
            asdx::ShaderHelper::PS_4_0,
            &pBlob );

        if ( FAILED( hr ) )
        {
            ELOG( "Error : Shader Compile Failed." );
            return false;
        }

        hr = m_pDevice->CreatePixelShader(
            pBlob->GetBufferPointer(),
            pBlob->GetBufferSize(),
            nullptr,
            &m_pPS );

        ASDX_RELEASE( pBlob );

        if ( FAILED( hr ) )
        {
            ELOG( "Error : ID3D11Device::CreatePixelShader() Failed." );
            return false;
        }
    }

    // 定数バッファの生成.
    {
        D3D11_BUFFER_DESC desc;
        ZeroMemory( &desc, sizeof( desc ) );
        desc.Usage     = D3D11_USAGE_DEFAULT;
        desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        desc.ByteWidth = sizeof( CBForward );
        desc.CPUAccessFlags = 0;

        hr = m_pDevice->CreateBuffer( &desc, nullptr, &m_pCBMatrixForward );
        if ( FAILED( hr ) )
        {
            ELOG( "Error : ID3D11Device::CreateBuffer() Failed." );
            return false;
        }

    }

    return true;
}

//-----------------------------------------------------------------------------------
//      フォワードレンダリングの終了処理です.
//-----------------------------------------------------------------------------------
void SampleApp::TermForward()
{
    ASDX_RELEASE( m_pVS );
    ASDX_RELEASE( m_pPS );
    ASDX_RELEASE( m_pCBMatrixForward );
    m_Dosei.Term();
}

//-----------------------------------------------------------------------------------
//      初期化処理です.
//-----------------------------------------------------------------------------------
bool SampleApp::OnInit()
{
    if ( !m_Font.Init( m_pDevice, "../res/font/SetoMini-P.fnt", f32(m_Width), f32(m_Height) ) )
    { return false; }

    if ( !InitQuad() )
    { return false; }

    if ( !InitShadowState() )
    { return false; }

    if ( !InitForward() )
    { return false; }


    // カメラの設定.
    m_Camera.GetCamera().SetPosition( asdx::Vector3( 100.0f, 0.0f, 0.0f ) );
    m_Camera.GetCamera().SetTarget( asdx::Vector3( 0.0f, 0.0f, 0.0f ) );
    m_Camera.GetCamera().SetUpward( asdx::Vector3( 0.0f, 1.0f, 0.0f ) );
    m_Camera.GetCamera().Preset();
    m_Camera.GetCamera().Update();

    // カメラの設定.
    m_DebugCamera.GetCamera().SetPosition( asdx::Vector3( 0.0f, 0.0f, 100.0f ) );
    m_DebugCamera.GetCamera().SetTarget( asdx::Vector3( 0.0f, 0.0f, 0.0f ) );
    m_DebugCamera.GetCamera().SetUpward( asdx::Vector3( 0.0f, 1.0f, 0.0f ) );
    m_DebugCamera.GetCamera().Preset();
    m_DebugCamera.GetCamera().Update();


    // クリップ平面までの距離を設定.
    f32 cameraNear = 0.1f;
    f32 cameraFar  = 1000.0f;
    f32 cameraFov  = asdx::F_PIDIV4;

    // ビュー・射影行列を求めておく.
    m_View = m_Camera.GetView();
    m_Proj = asdx::Matrix::CreatePerspectiveFieldOfView(
        cameraFov,
        m_AspectRatio,
        cameraNear,
        cameraFar );

    m_DebugCameraView = m_DebugCamera.GetView();
    m_DebugCameraProj = asdx::Matrix::CreatePerspectiveFieldOfView(
        cameraFov,
        m_AspectRatio,
        cameraNear,
        cameraFar );

    // ライトの設定.
    m_LightDir = asdx::Vector3( 0.0f, -1.0f, 0.0f );
    m_LightDir.Normalize();

    m_SdwMgr.SetLightDir( m_LightDir );
    asdx::Vector3 dir = m_Camera.GetCamera().GetTarget() - m_Camera.GetCamera().GetPosition();
    dir.Normalize();
    m_SdwMgr.SetCameraNear( cameraNear );
    m_SdwMgr.SetCameraFar( cameraFar );
    m_SdwMgr.SetCameraFov( cameraFov );
    m_SdwMgr.SetCameraPos( m_Camera.GetCamera().GetPosition() );
    m_SdwMgr.SetCameraDir( dir );
    m_SdwMgr.SetCameraUp( m_Camera.GetCamera().GetUpward() );
    m_SdwMgr.SetCameraView( m_View );
    m_SdwMgr.SetCameraProj( m_Proj );

    m_SdwMgr.SetCasterBox( m_Box_Dosei );

    return true;
}

//-----------------------------------------------------------------------------------
//      終了処理です.
//-----------------------------------------------------------------------------------
void SampleApp::OnTerm()
{
    TermForward();
    TermQuad();
    TermShadowState();
    m_Font.Term();
}

//-----------------------------------------------------------------------------------
//      投影行列をを生成する.
//-----------------------------------------------------------------------------------
asdx::Matrix SampleApp::CreateScreenMatrix
(
    const f32 x,
    const f32 y,
    const f32 sx,
    const f32 sy
)
{
    f32 dx = 2.0f / m_Width;
    f32 dy = 2.0f / m_Height;
    f32 tx = -1.0f + dx * ( x + sx );
    f32 ty = -1.0f + dy * ( y + sy );
    return asdx::Matrix(
        dx * sx,     0.0f,        0.0f,        0.0f,
        0.0f,        dy * sy,     0.0f,        0.0f,
        0.0f,        0.0f,        1.0f,        0.0f,
        tx,          ty,          0.0f,        1.0f ); 
}

//-----------------------------------------------------------------------------------
//      矩形を描画します.
//-----------------------------------------------------------------------------------
void SampleApp::DrawQuad( ID3D11ShaderResourceView* pSRV )
{
    m_pDeviceContext->VSSetShader( m_pQuadVS,   nullptr, 0 );
    m_pDeviceContext->GSSetShader( nullptr,     nullptr, 0 );
    m_pDeviceContext->PSSetShader( m_pQuadPS,   nullptr, 0 );
    m_pDeviceContext->DSSetShader( nullptr,     nullptr, 0 );
    m_pDeviceContext->HSSetShader( nullptr,     nullptr, 0 );

    QuadParam param;

    // レンダーターゲット.
    m_pDeviceContext->PSSetShaderResources( 0, 1, &pSRV );
    m_pDeviceContext->PSSetSamplers( 0, 1, &m_pQuadSmp );
    {
        param.matrix = CreateScreenMatrix( 0.0f, 0.0f, 75.0f, 75.0f );
        m_pDeviceContext->UpdateSubresource( m_pQuadCB, 0, nullptr, &param, 0, 0 );
        m_pDeviceContext->VSSetConstantBuffers( 0, 1, &m_pQuadCB );
        m_pDeviceContext->PSSetConstantBuffers( 0, 1, &m_pQuadCB );

        m_Quad.Draw( m_pDeviceContext );
    }

    ID3D11SamplerState*         pNullSmp = nullptr;
    ID3D11ShaderResourceView*   pNullSRV = nullptr;
    m_pDeviceContext->PSSetSamplers( 0, 1, &pNullSmp );
    m_pDeviceContext->PSSetShaderResources( 0, 1, &pNullSRV );

    m_pDeviceContext->VSSetShader( nullptr, nullptr, 0 );
    m_pDeviceContext->GSSetShader( nullptr, nullptr, 0 );
    m_pDeviceContext->PSSetShader( nullptr, nullptr, 0 );
    m_pDeviceContext->DSSetShader( nullptr, nullptr, 0 );
    m_pDeviceContext->HSSetShader( nullptr, nullptr, 0 );
}

//---------------------------------------------------------------------------------------
//      描画時の処理です.
//---------------------------------------------------------------------------------------
void SampleApp::OnFrameRender( asdx::FrameEventParam& param )
{
    // シャドウマップに描画する.
    DrawToShadowMap();

    // レンダーターゲットビュー・深度ステンシルビューを取得.
    {
        ID3D11RenderTargetView* pRTV = nullptr;
        ID3D11DepthStencilView* pDSV = nullptr;
        pRTV = m_RT.GetRTV();
        pDSV = m_DST.GetDSV();

        // NULLチェック.
        if ( pRTV == nullptr )
        { return; }
        if ( pDSV == nullptr )
        { return; }
    
        // 出力マネージャに設定.
        m_pDeviceContext->OMSetRenderTargets( 1, &pRTV, pDSV );

        D3D11_VIEWPORT viewport;
        viewport.TopLeftX = 0.0f;
        viewport.TopLeftY = 0.0f;
        viewport.Width    = f32(m_Width);
        viewport.Height   = f32(m_Height);
        viewport.MinDepth = 0.0f;
        viewport.MaxDepth = 1.0f;
        m_pDeviceContext->RSSetViewports( 1, &viewport );

        // クリア処理.
        m_pDeviceContext->ClearRenderTargetView( pRTV, m_ClearColor );
        m_pDeviceContext->ClearDepthStencilView( pDSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0 );

        m_pDeviceContext->VSSetShader( m_pVS,   nullptr, 0 );
        m_pDeviceContext->GSSetShader( nullptr, nullptr, 0 );   // ジオメトリシェーダつかったら遅かったので，使わない.
        m_pDeviceContext->PSSetShader( m_pPS,   nullptr, 0 );
        m_pDeviceContext->HSSetShader( nullptr, nullptr, 0 );
        m_pDeviceContext->DSSetShader( nullptr, nullptr, 0 );

        //　定数バッファ更新.
        asdx::Matrix lightRot = asdx::Matrix::CreateRotationX( m_LightRotX )
                              * asdx::Matrix::CreateRotationY( m_LightRotY );
        CBForward cbParam;

        cbParam.World     = asdx::Matrix::CreateScale( 0.25f );
        if ( m_CameraMode == 0 )
        {
            cbParam.View      = m_View;
            cbParam.Proj      = m_Proj;
        }
        else if ( m_CameraMode == 1 )
        {
            cbParam.View      = m_SdwMgr.GetLightView();
            cbParam.Proj      = m_SdwMgr.GetLightProj();
        }
        else 
        {
            cbParam.View = m_DebugCameraView;
            cbParam.Proj = m_DebugCameraProj;
        }
        cbParam.CameraPos = m_Camera.GetCamera().GetPosition();
        cbParam.LightDir  = asdx::Vector3::Transform( m_LightDir, lightRot );
        cbParam.Shadow    = m_SdwMgr.GetLightViewProj() * SHADOW_BIAS;

        m_pDeviceContext->UpdateSubresource( m_pCBMatrixForward, 0, nullptr, &cbParam, 0, 0 );

        ID3D11ShaderResourceView* pSRV = m_SdwMgr.GetSRV( asdx::ShadowMgr::DEPTH_BUFFER );
        // 定数バッファ設定.
        m_pDeviceContext->VSSetConstantBuffers( 1, 1, &m_pCBMatrixForward ); 
        m_pDeviceContext->PSSetShaderResources( 3, 1, &pSRV );

        ID3D11SamplerState* pSmp = m_SdwMgr.GetSmp();
        // サンプラーステートを設定.
        m_pDeviceContext->PSSetSamplers( 3, 1, &pSmp );

        // 描画キック.
        m_Dosei.Draw ( m_pDeviceContext );

        ID3D11ShaderResourceView* pNullSRV[4] = { nullptr, nullptr, nullptr, nullptr };
        m_pDeviceContext->PSSetShaderResources( 0, 4, pNullSRV );

        m_pDeviceContext->VSSetShader( nullptr, nullptr, 0 );
        m_pDeviceContext->PSSetShader( nullptr, nullptr, 0 );

        // FPS描画.
        {
            m_Font.Begin( m_pDeviceContext);
            m_Font.DrawStringArg( 10, 10, "FPS : %.2f", param.FPS );
            m_Font.DrawStringArg( 10, 30, "Light Rotation X : %f", m_LightRotX );
            m_Font.DrawStringArg( 10, 50, "Light Rotation Y : %f", m_LightRotY );
            m_Font.End( m_pDeviceContext );
        }

        // 左下にシャドウマップをデバッグ描画.
        if ( m_ShowTexture )
        {
            ID3D11ShaderResourceView* pSRV = m_SdwMgr.GetSRV( asdx::ShadowMgr::DEPTH_BUFFER );
            DrawQuad( pSRV );
        }
    }

    // コマンドを実行して，画面に表示.
    Present( 0 );
}

//---------------------------------------------------------------------------------------
//      シャドウマップに描画する.
//---------------------------------------------------------------------------------------
void SampleApp::DrawToShadowMap()
{
    m_SdwMgr.SetCameraView( m_View );
    m_SdwMgr.SetCameraProj( m_Proj );

    m_SdwMgr.ComputeMatrix();
    m_SdwMgr.Begin( m_pDeviceContext );

    m_SdwMgr.SetWorld( asdx::Matrix::CreateScale( 0.25f ) );
    m_SdwMgr.UpdateCB( m_pDeviceContext );

    m_Dosei.Draw ( m_pDeviceContext );

    m_SdwMgr.End( m_pDeviceContext );
}

//---------------------------------------------------------------------------------------
//      フレーム遷移時の処理です.
//---------------------------------------------------------------------------------------
void SampleApp::OnFrameMove( asdx::FrameEventParam& param )
{
    // ビュー行列を更新.
    m_View = m_Camera.GetView();
    m_DebugCameraView = m_DebugCamera.GetView();

    asdx::Vector3 dir = m_Camera.GetCamera().GetTarget() - m_Camera.GetCamera().GetPosition();
    dir.Normalize();
    m_SdwMgr.SetCameraPos( m_Camera.GetCamera().GetPosition() );
    m_SdwMgr.SetCameraDir( dir );
    m_SdwMgr.SetCameraUp( m_Camera.GetCamera().GetUpward() );

    // ライトの回転処理.
    asdx::Matrix  lightRot = asdx::Matrix::CreateRotationX( m_LightRotX ) 
                           * asdx::Matrix::CreateRotationY( m_LightRotY );

    // ライトの方向ベクトルを求める.
    asdx::Vector3 lightDir = asdx::Vector3::Transform( m_LightDir, lightRot );
    lightDir.Normalize();

    m_SdwMgr.SetLightDir( lightDir );
}

//---------------------------------------------------------------------------------------
//      マウスインベント時の処理です.
//---------------------------------------------------------------------------------------
void SampleApp::OnMouse( const asdx::MouseEventParam& param )
{
    if ( m_CameraMode == 0 )
    {
        // カメラ処理.
        m_Camera.OnMouse( 
            param.X,
            param.Y,
            param.WheelDelta,
            param.IsLeftButtonDown,
            param.IsRightButtonDown,
            param.IsMiddleButtonDown,
            param.IsSideButton1Down,
            param.IsSideButton2Down
        );
    }
    else if ( m_CameraMode == 2 )
    {
        // カメラ処理.
        m_DebugCamera.OnMouse( 
            param.X,
            param.Y,
            param.WheelDelta,
            param.IsLeftButtonDown,
            param.IsRightButtonDown,
            param.IsMiddleButtonDown,
            param.IsSideButton1Down,
            param.IsSideButton2Down
        );
    }
}

//---------------------------------------------------------------------------------------
//      キー入力時の処理.
//---------------------------------------------------------------------------------------
void SampleApp::OnKey( const asdx::KeyEventParam& param )
{
    f32 speed = 0.05f;
    if ( param.IsKeyDown )
    {
        switch( param.KeyCode )
        {
        case VK_UP:
            { m_LightRotX += speed; }
            break;

        case VK_DOWN:
            { m_LightRotX -= speed; }
            break;

        case VK_LEFT:
            { m_LightRotY -= speed; }
            break;

        case VK_RIGHT:
            { m_LightRotY += speed; }
            break;

        case 'C':
            {
                m_CameraMode = 0;
            }
            break;

        case 'L':
            {
                m_CameraMode = 1;
            }
            break;

        case 'D':
            {
                m_CameraMode = 2;
            }
            break;

        case 'R':
            {
                m_LightRotX = asdx::F_PIDIV4;
                m_LightRotY = asdx::F_PIDIV2;
            }
            break;

        case 'H':
            { m_ShowTexture = (!m_ShowTexture); }
            break;
        }
    }
}