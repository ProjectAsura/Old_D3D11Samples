//-----------------------------------------------------------------------------
// File : App.h
// Desc : Application.
// Copyright(c) Project Asura. All right reserved.
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <asdxApp.h>
#include <asdxTexture.h>
#include <asdxShader.h>
#include <asdxConstantBuffer.h>
#include <asdxSprite.h>


///////////////////////////////////////////////////////////////////////////////
// App class
///////////////////////////////////////////////////////////////////////////////
class App : public asdx::Application
{
    //=========================================================================
    // list of friend classes and methods.
    //=========================================================================
    /* NOTHING */

public:
    //=========================================================================
    // public variables.
    //=========================================================================
    /* NOTHING */

    //=========================================================================
    // public methods.
    //=========================================================================
    App();
    ~App();

private:
    //=========================================================================
    // private variables.
    //=========================================================================
    asdx::Texture2D         m_Texture;
    asdx::ConstantBuffer    m_CB;
    asdx::ComputeShader     m_CS;
    asdx::Sprite            m_Sprite;
    uint32_t                m_ThreadCountX;
    uint32_t                m_ThreadCountY;
    uint32_t                m_ThreadCountZ;
    uint32_t                m_TextureWidth;
    uint32_t                m_TextureHeight;

    asdx::RefPtr<ID3D11Texture2D>           m_ComputeTex;
    asdx::RefPtr<ID3D11UnorderedAccessView> m_ComputeUAV;
    asdx::RefPtr<ID3D11ShaderResourceView>  m_ComputeSRV;

    //=========================================================================
    // private methods.
    //=========================================================================
    bool OnInit() override;
    void OnTerm() override;
    void OnFrameRender(asdx::FrameEventArgs& param) override;
    void OnResize(const asdx::ResizeEventArgs& param) override;
};