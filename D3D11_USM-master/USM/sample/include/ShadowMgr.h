//------------------------------------------------------------------------------------------
// File : ShadowMapMgr.h
// Desc : Shadow Map Manager Module.
// Copyright(c) Project Asura. All right reserved.
//------------------------------------------------------------------------------------------

#ifndef __SHADOW_MGR_H__
#define __SHADOW_MGR_H__

//-------------------------------------------------------------------------------------------
// Includes
//-------------------------------------------------------------------------------------------
#include <asdxTypedef.h>
#include <asdxRenderTarget.h>
#include <asdxDepthStencilTarget.h>
#include <asdxMath.h>
#include <asdxGeometry.h>
#include <asdxConstantBuffer.h>
#include <asdxOnb.h>


namespace asdx {

/////////////////////////////////////////////////////////////////////////////////////////////
// ShadowMgr class
/////////////////////////////////////////////////////////////////////////////////////////////
class ShadowMgr
{
    //=======================================================================================
    // list of friend classes and methods.
    //=======================================================================================
    /* NOTHING */

public:
    enum BUFFER_TYPE
    {
        DEPTH_BUFFER = 0,
        COLOR_BUFFER = 1,
    };

    //=======================================================================================
    // public variables.
    //=======================================================================================
    /* NOTHING */

    //=======================================================================================
    // public methods.
    //=======================================================================================
    ShadowMgr();
    virtual ~ShadowMgr();

    bool Init( ID3D11Device* pDevice, const u32 width, const u32 height );
    void Term();

    void Begin( ID3D11DeviceContext* pDeviceContext );
    void End  ( ID3D11DeviceContext* pDeviceContext );
    void UpdateCB( ID3D11DeviceContext* pDeviceContext );
    void ComputeMatrix();

    void SetCameraPos( const asdx::Vector3& pos );
    void SetCameraDir( const asdx::Vector3& dir );
    void SetCameraUp ( const asdx::Vector3& upward );
    void SetCameraNear( const f32 nearClip );
    void SetCameraFar ( const f32 farClip ); 
    void SetCameraFov ( const f32 fovy );
    void SetWorld( const asdx::Matrix& world );
    void SetCameraView( const asdx::Matrix& view );
    void SetCameraProj( const asdx::Matrix& proj );
    void SetLightDir( const asdx::Vector3& lightDir );
    void SetCasterBox( const asdx::BoundingBox& box );

    asdx::Matrix GetLightView() const
    { return m_LightView; }

    asdx::Matrix GetLightProj() const
    { return m_LightProj; }

    asdx::Matrix GetLightViewProj() const
    { return m_LightView * m_LightProj; }

    ID3D11ShaderResourceView* GetSRV( BUFFER_TYPE type )
    {
        if ( type == DEPTH_BUFFER )
        { return m_DST.GetSRV(); }
        else if ( type == COLOR_BUFFER )
        { return m_RT.GetSRV(); }

        return nullptr;
    }

    ID3D11SamplerState* GetSmp()
    { return m_pSmp; }

protected:
    //=======================================================================================
    // protected variables.
    //=======================================================================================
    /* NOTHING */

    //=======================================================================================
    // protected methods.
    //=======================================================================================
    asdx::Matrix UnitCubeClip( const asdx::Vector3& mini, const asdx::Vector3& maxi );

    void ComputeMatrixUSM();

private:
    //=======================================================================================
    // private variables.
    //=======================================================================================
    asdx::RenderTarget2D        m_RT;
    asdx::DepthStencilTarget    m_DST;
    asdx::ConstantBuffer        m_CBGenShadow;
    ID3D11VertexShader*         m_pVSGenShadow;
    ID3D11PixelShader*          m_pPSGenShadow;
    ID3D11DepthStencilState*    m_pDSS;
    ID3D11SamplerState*         m_pSmp;
    D3D11_VIEWPORT              m_Viewport;

    asdx::Vector3               m_LightDir;
    asdx::OrthonormalBasis      m_LightBasis;

    asdx::Vector3               m_CameraPos;
    asdx::Vector3               m_CameraDir;
    asdx::Vector3               m_CameraUp;
    f32                         m_CameraNear;
    f32                         m_CameraFar;
    f32                         m_CameraFov;

    asdx::Matrix                m_World;
    asdx::Matrix                m_CameraView;
    asdx::Matrix                m_CameraProj;

    asdx::Matrix                m_LightView;
    asdx::Matrix                m_LightProj;

    asdx::BoundingBox           m_CasterBox;

    //=======================================================================================
    // private methods.
    //=======================================================================================
    /* NOTHING */
};


}// namespace asdx


#endif//__SHADOW_MGR_H__
