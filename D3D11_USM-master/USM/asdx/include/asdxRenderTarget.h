﻿//------------------------------------------------------------------------------------------
// File : asdxRenderTarget.h
// Desc : RenderTarget2D Module.
// Copyright(c) Project Asura. All right reserved.
//------------------------------------------------------------------------------------------

#ifndef __ASDX_RENDER_TARGET_H__
#define __ASDX_RENDER_TARGET_H__

//------------------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------------------
#include <d3d11.h>


namespace asdx {

//////////////////////////////////////////////////////////////////////////////////////
// RenderTarget2D class
//////////////////////////////////////////////////////////////////////////////////////
class RenderTarget2D
{
    //================================================================================
    // list of friend classes and methods.
    //================================================================================
    /* NOTHING */

public:
    //////////////////////////////////////////////////////////////////////////////////
    // Description structure
    //////////////////////////////////////////////////////////////////////////////////
    struct Description
    {
        UINT                Width;          //!< 横幅です.
        UINT                Height;         //!< 縦幅です.
        UINT                MipLevels;      //!< ミップレベルです.
        UINT                ArraySize;      //!< 配列サイズです.
        DXGI_FORMAT         Format;         //!< フォーマットです.
        DXGI_SAMPLE_DESC    SampleDesc;     //!< サンプルの概要です.
        UINT                CPUAccessFlags; //!< CPUアクセスフラグです.
        UINT                MiscFlags;      //!< フラグです.

        //-----------------------------------------------------------------------------
        //! @brief      コンストラクタです.
        //-----------------------------------------------------------------------------
        Description()
        : Width         ( 0 )
        , Height        ( 0 )
        , MipLevels     ( 0 )
        , ArraySize     ( 0 )
        , Format        ( DXGI_FORMAT_UNKNOWN )
        , CPUAccessFlags( 0 )
        , MiscFlags     ( 0 )
        {
            SampleDesc.Count   = 0;
            SampleDesc.Quality = 0;
        }
    };

    //================================================================================
    // public variables
    //================================================================================
    /* NOTHING */

    //================================================================================
    // public methods
    //================================================================================

    //--------------------------------------------------------------------------------
    //! @brief      コンストラクタです.
    //--------------------------------------------------------------------------------
    RenderTarget2D();

    //--------------------------------------------------------------------------------
    //! @brief      デストラクタです.
    //--------------------------------------------------------------------------------
    virtual ~RenderTarget2D();

    //--------------------------------------------------------------------------------
    //! @brief      バックバッファから2次元描画ターゲットを生成します.
    //!
    //! @param [in]     pDevice     デバイスです.
    //! @param [in]     pSwapChain  スワップチェインです.
    //! @retval true    描画ターゲットの生成に成功.
    //! @retval false   描画ターゲットの生成に失敗.
    //--------------------------------------------------------------------------------
    bool CreateFromBackBuffer( ID3D11Device* pDevice, IDXGISwapChain* pSwapChain );

    //--------------------------------------------------------------------------------
    //! @brief      描画ターゲットを生成します.
    //!
    //! @param [in]     pDevice     デバイスです.
    //! @param [in]     desc        概要です.
    //! @retval true    描画ターゲットの生成に成功.
    //! @retval false   描画ターゲットの生成に失敗.
    //--------------------------------------------------------------------------------
    bool Create( ID3D11Device* pDevice, const Description& desc );

    //--------------------------------------------------------------------------------
    //! @brief      テクスチャを取得します.
    //!
    //! @return     テクスチャを返却します.
    //--------------------------------------------------------------------------------
    ID3D11Texture2D* GetTexture() const;

    //--------------------------------------------------------------------------------
    //! @brief      シェーダリソースビューを取得します.
    //!
    //! @return     シェーダリソースビューを返却します.
    //--------------------------------------------------------------------------------
    ID3D11ShaderResourceView* GetSRV() const;

    //--------------------------------------------------------------------------------
    //! @brief      レンダーターゲットビューを取得します.
    //!
    //! @return     レンダーターゲットビューを返却します.
    //--------------------------------------------------------------------------------
    ID3D11RenderTargetView* GetRTV();

    //--------------------------------------------------------------------------------
    //! @brief      レンダーターゲットビューを取得します.
    //!
    //! @return     レンダーターゲットビューを返却します.
    //--------------------------------------------------------------------------------
    ID3D11RenderTargetView* const GetRTV() const;

    //--------------------------------------------------------------------------------
    //! @brief      解放処理を行います.
    //--------------------------------------------------------------------------------
    void Release();

protected:
    //================================================================================
    // protected variables
    //================================================================================
    /* NOTHING */

    //================================================================================
    // protected methods
    //================================================================================
    /* NOTHING */

private:
    //================================================================================
    // private variables
    //================================================================================
    ID3D11Texture2D*            m_pTexture;     //!< テクスチャです.
    ID3D11ShaderResourceView*   m_pSRV;         //!< シェーダリソースビューです.
    ID3D11RenderTargetView*     m_pRTV;         //!< レンダーターゲットビューです.

    //================================================================================
    // private methods.
    //================================================================================
    RenderTarget2D  ( const RenderTarget2D& );  // アクセス禁止.
    void operator = ( const RenderTarget2D& );  // アクセス禁止.
};

} // namespace asdx

#endif//__ASDX_RENDER_TARGET_H__