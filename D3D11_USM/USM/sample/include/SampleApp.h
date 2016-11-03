//-----------------------------------------------------------------------------------
// File : sampleApp.h
// Desc : Sample Application Module.
// Copyright(c) Project Asura. All right reserved.
//-----------------------------------------------------------------------------------

#ifndef __SAMPLE_APP_H__
#define __SAMPLE_APP_H__

//------------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------------
#include <asdxTypedef.h>
#include <asdxApp.h>
#include <asdxQuadRenderer.h>
#include <asdxFont.h>
#include <asdxMesh.h>
#include <asdxCameraUpdater.h>
#include <ShadowMgr.h>


//////////////////////////////////////////////////////////////////////////////////////
// SampleApp class
//////////////////////////////////////////////////////////////////////////////////////
class SampleApp : public asdx::Application
{
    //================================================================================
    // list of friend classes and methods.
    //================================================================================
    /* NOTHING */

public:
    //================================================================================
    // public variables.
    //================================================================================

    //================================================================================
    // public methods.
    //================================================================================
    SampleApp();
    virtual ~SampleApp();

protected:
    //================================================================================
    // protected variables.
    //================================================================================

    //================================================================================
    // protected methods.
    //================================================================================
    virtual bool OnInit();
    virtual void OnTerm();
    virtual void OnFrameRender( asdx::FrameEventParam& );

    bool InitQuad();
    void TermQuad();
    void DrawQuad( ID3D11ShaderResourceView* pSRV );
    bool InitShadowState();
    void TermShadowState();
    void DrawToShadowMap();
    bool InitMesh();
    void TermMesh();
    bool InitForward();
    void TermForward();
    void OnFrameMove( asdx::FrameEventParam& param );
    void OnMouse    ( const asdx::MouseEventParam&  param );
    void OnKey      ( const asdx::KeyEventParam& param );

    asdx::Matrix CreateScreenMatrix(
        const f32 x,
        const f32 y,
        const f32 sx,
        const f32 sy );

private:
    //================================================================================
    // private variables.
    //================================================================================
    asdx::QuadRenderer          m_Quad;
    ID3D11VertexShader*         m_pQuadVS;
    ID3D11PixelShader*          m_pQuadPS;
    ID3D11Buffer*               m_pQuadCB;
    ID3D11SamplerState*         m_pQuadSmp;
    asdx::Font                  m_Font;
    asdx::CameraUpdater         m_Camera;

    ID3D11VertexShader*         m_pVS;
    ID3D11PixelShader*          m_pPS;
    ID3D11Buffer*               m_pCBMatrixForward;

    asdx::Mesh                  m_Dosei;
    asdx::BoundingBox           m_Box_Dosei;

    asdx::Matrix                m_View;
    asdx::Matrix                m_Proj;
    asdx::CameraUpdater         m_DebugCamera;
    asdx::Matrix                m_DebugCameraView;
    asdx::Matrix                m_DebugCameraProj;

    asdx::Vector3               m_LightDir;
    f32                         m_LightRotX;
    f32                         m_LightRotY;
    bool                        m_ShowTexture;
    s32                         m_CameraMode;

    asdx::ShadowMgr             m_SdwMgr;


    //================================================================================
    // private methods.
    //================================================================================
    SampleApp       ( const SampleApp& );
    void operator = ( const SampleApp& );
};

#endif//__SAMPLE_APP_H__
