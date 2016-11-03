﻿//-------------------------------------------------------------------------------------------------
// File : asdxTarget.h
// Desc : Render Target Module.
// Copyright(c) Project Asura. All right reserved.
//-------------------------------------------------------------------------------------------------

#ifndef __ASDX_TARGET_H__
#define __ASDX_TARGET_H__

//-------------------------------------------------------------------------------------------------
// Includes
//-------------------------------------------------------------------------------------------------
#include <d3d11.h>
#include <asdxRef.h>


namespace asdx {

///////////////////////////////////////////////////////////////////////////////////////////////////
// TargetDesc2D structure
///////////////////////////////////////////////////////////////////////////////////////////////////
struct TargetDesc2D
{
    UINT                Width;              //!< 横幅です.
    UINT                Height;             //!< 縦幅です.
    UINT                ArraySize;          //!< 配列サイズです.
    UINT                MipLevels;          //!< ミップレベル数です.
    DXGI_FORMAT         Format;             //!< フォーマットです.
    DXGI_SAMPLE_DESC    SampleDesc;         //!< サンプル設定です.
    UINT                CPUAccessFlags;     //!< CPUアクセスフラグです.
    UINT                MiscFlags;          //!< フラグです.

    //---------------------------------------------------------------------------------------------
    //! @brief      コンストラクタです.
    //---------------------------------------------------------------------------------------------
    TargetDesc2D()
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


///////////////////////////////////////////////////////////////////////////////////////////////////
// TargetDesc3D structure
///////////////////////////////////////////////////////////////////////////////////////////////////
struct TargetDesc3D
{
    UINT                Width;              //!< 横幅です.
    UINT                Height;             //!< 縦幅です.
    UINT                Depth;              //!< 奥行です.
    UINT                MipLevels;          //!< ミップレベル数です.
    DXGI_FORMAT         Format;             //!< フォーマットです.
    UINT                CPUAccessFlags;     //!< CPUアクセスフラグです.
    UINT                MiscFlags;          //!< フラグです.

    //---------------------------------------------------------------------------------------------
    //! @brief      コンストラクタです.
    //---------------------------------------------------------------------------------------------
    TargetDesc3D()
    : Width         ( 0 )
    , Height        ( 0 )
    , Depth         ( 0 )
    , MipLevels     ( 0 )
    , Format        ( DXGI_FORMAT_UNKNOWN )
    , CPUAccessFlags( 0 )
    , MiscFlags     ( 0 )
    { /* DO_NOTHING */ }
};


///////////////////////////////////////////////////////////////////////////////////////////////////
// ColorTarget2D class
///////////////////////////////////////////////////////////////////////////////////////////////////
class ColorTarget2D : private NonCopyable
{
    //=============================================================================================
    // list of friend classes and methods.
    //=============================================================================================
    /* NOTHING */

public:
    //=============================================================================================
    // public variables
    //=============================================================================================
    /* NOTHING */

    //=============================================================================================
    // public methods
    //=============================================================================================

    //---------------------------------------------------------------------------------------------
    //! @brief      コンストラクタです.
    //---------------------------------------------------------------------------------------------
    ColorTarget2D();

    //---------------------------------------------------------------------------------------------
    //! @brief      デストラクタです.
    //---------------------------------------------------------------------------------------------
    virtual ~ColorTarget2D();

    //---------------------------------------------------------------------------------------------
    //! @brief      バックバッファから2次元描画ターゲットを生成します.
    //!
    //! @param [in]     pDevice     デバイスです.
    //! @param [in]     pSwapChain  スワップチェインです.
    //! @retval true    描画ターゲットの生成に成功.
    //! @retval false   描画ターゲットの生成に失敗.
    //---------------------------------------------------------------------------------------------
    bool CreateFromBackBuffer( ID3D11Device* pDevice, IDXGISwapChain* pSwapChain );

    //---------------------------------------------------------------------------------------------
    //! @brief      描画ターゲットを生成します.
    //!
    //! @param [in]     pDevice     デバイスです.
    //! @param [in]     desc        概要です.
    //! @retval true    描画ターゲットの生成に成功.
    //! @retval false   描画ターゲットの生成に失敗.
    //---------------------------------------------------------------------------------------------
    bool Create( ID3D11Device* pDevice, const TargetDesc2D& desc );

    //---------------------------------------------------------------------------------------------
    //! @brief      テクスチャを取得します.
    //!
    //! @return     テクスチャを返却します.
    //---------------------------------------------------------------------------------------------
    ID3D11Texture2D* const GetResource() const;

    //---------------------------------------------------------------------------------------------
    //! @brief      シェーダリソースビューを取得します.
    //!
    //! @return     シェーダリソースビューを返却します.
    //---------------------------------------------------------------------------------------------
    ID3D11ShaderResourceView* const GetShaderResource() const;

    //---------------------------------------------------------------------------------------------
    //! @brief      レンダーターゲットビューを取得します.
    //!
    //! @return     レンダーターゲットビューを返却します.
    //---------------------------------------------------------------------------------------------
    ID3D11RenderTargetView* const GetTargetView() const;

    //---------------------------------------------------------------------------------------------
    //! @brief      構成設定を取得します.
    //!
    //! @return     構成設定を返却します.
    //---------------------------------------------------------------------------------------------
    TargetDesc2D GetDesc() const;


    //---------------------------------------------------------------------------------------------
    //! @brief      解放処理を行います.
    //---------------------------------------------------------------------------------------------
    void Release();

protected:
    //=============================================================================================
    // protected variables
    //=============================================================================================
    /* NOTHING */

    //=============================================================================================
    // protected methods
    //=============================================================================================
    /* NOTHING */

private:
    //=============================================================================================
    // private variables
    //=============================================================================================
    RefPtr<ID3D11Texture2D>            m_pTexture;     //!< テクスチャです.
    RefPtr<ID3D11ShaderResourceView>   m_pSRV;         //!< シェーダリソースビューです.
    RefPtr<ID3D11RenderTargetView>     m_pRTV;         //!< レンダーターゲットビューです.
    TargetDesc2D                       m_Desc;         //!< 構成設定です.

    //=============================================================================================
    // private methods.
    //=============================================================================================
    /* NOTHING */
};


///////////////////////////////////////////////////////////////////////////////////////////////////
// ColorTarget3D class
///////////////////////////////////////////////////////////////////////////////////////////////////
class ColorTarget3D : private NonCopyable
{
    //=============================================================================================
    // list of friend classes and methods.
    //=============================================================================================
    /* NOTHING */

public:
    //=============================================================================================
    // public variables
    //=============================================================================================
    /* NOTHING */

    //=============================================================================================
    // public methods
    //=============================================================================================

    //---------------------------------------------------------------------------------------------
    //! @brief      コンストラクタです.
    //---------------------------------------------------------------------------------------------
    ColorTarget3D();

    //---------------------------------------------------------------------------------------------
    //! @brief      デストラクタです.
    //---------------------------------------------------------------------------------------------
    virtual ~ColorTarget3D();

    //---------------------------------------------------------------------------------------------
    //! @brief      描画ターゲットを生成します.
    //!
    //! @param [in]     pDevice     デバイスです.
    //! @param [in]     desc        概要です.
    //! @retval true    描画ターゲットの生成に成功.
    //! @retval false   描画ターゲットの生成に失敗.
    //---------------------------------------------------------------------------------------------
    bool Create( ID3D11Device* pDevice, const TargetDesc3D& desc );

    //---------------------------------------------------------------------------------------------
    //! @brief      テクスチャを取得します.
    //!
    //! @return     テクスチャを返却します.
    //---------------------------------------------------------------------------------------------
    ID3D11Texture3D* const GetResource() const;

    //---------------------------------------------------------------------------------------------
    //! @brief      シェーダリソースビューを取得します.
    //!
    //! @return     シェーダリソースビューを返却します.
    //---------------------------------------------------------------------------------------------
    ID3D11ShaderResourceView* const GetShaderResource() const;

    //---------------------------------------------------------------------------------------------
    //! @brief      レンダーターゲットビューを取得します.
    //!
    //! @return     レンダーターゲットビューを返却します.
    //---------------------------------------------------------------------------------------------
    ID3D11RenderTargetView* const GetTargetView() const;

    //---------------------------------------------------------------------------------------------
    //! @brief      構成設定を取得します.
    //!
    //! @return     構成設定を返却します.
    //---------------------------------------------------------------------------------------------
    TargetDesc3D GetDesc() const;

    //---------------------------------------------------------------------------------------------
    //! @brief      解放処理を行います.
    //---------------------------------------------------------------------------------------------
    void Release();

protected:
    //=============================================================================================
    // protected variables
    //=============================================================================================
    /* NOTHING */

    //=============================================================================================
    // protected methods
    //=============================================================================================
    /* NOTHING */

private:
    //=============================================================================================
    // private variables
    //=============================================================================================
    RefPtr<ID3D11Texture3D>            m_pTexture;     //!< テクスチャです.
    RefPtr<ID3D11ShaderResourceView>   m_pSRV;         //!< シェーダリソースビューです.
    RefPtr<ID3D11RenderTargetView>     m_pRTV;         //!< レンダーターゲットビューです.
    TargetDesc3D                       m_Desc;         //!< 構成設定です.

    //=============================================================================================
    // private methods.
    //=============================================================================================
    /* NOTHING */
};

///////////////////////////////////////////////////////////////////////////////////////////////////
// DepthTarget2D class
///////////////////////////////////////////////////////////////////////////////////////////////////
class DepthTarget2D : private NonCopyable
{
public:
    //=============================================================================================
    // public variables.
    //=============================================================================================
    /* NOTHING */

    //=============================================================================================
    // public methods.
    //=============================================================================================

    //---------------------------------------------------------------------------------------------
    //! @brief      コンストラクタです.
    //---------------------------------------------------------------------------------------------
    DepthTarget2D();

    //---------------------------------------------------------------------------------------------
    //! @brief      デストラクタです.
    //---------------------------------------------------------------------------------------------
    virtual ~DepthTarget2D();

    //---------------------------------------------------------------------------------------------
    //! @brief      深度ステンシルバッファを生成します.
    //!
    //! @param [in]     pDevice     デバイスです.
    //! @param [in]     desc        概要です.
    //! @retval true    深度ステンシルバッファの生成に成功.
    //! @retval false   深度ステンシルバッファの生成に失敗.
    //---------------------------------------------------------------------------------------------
    bool Create( ID3D11Device* pDevice, const TargetDesc2D& desc );

    //---------------------------------------------------------------------------------------------
    //! @brief      テクスチャを取得します.
    //!
    //! @retrun     テクスチャを返却します.
    //---------------------------------------------------------------------------------------------
    ID3D11Texture2D* const GetResource() const;

    //---------------------------------------------------------------------------------------------
    //! @brief      シェーダリソースビューを取得します.
    //!
    //! @return     シェーダリソースビューを返却します.
    //---------------------------------------------------------------------------------------------
    ID3D11ShaderResourceView* const GetShaderResource() const;

    //---------------------------------------------------------------------------------------------
    //! @brief      深度ステンシルビューを取得します.
    //!
    //! @return     深度ステンシルビューを返却します.
    //---------------------------------------------------------------------------------------------
    ID3D11DepthStencilView* const GetTargetView() const;

    //---------------------------------------------------------------------------------------------
    //! @brief      構成設定を取得します.
    //!
    //! @return     構成設定を返却します.
    //---------------------------------------------------------------------------------------------
    TargetDesc2D GetDesc() const;

    //---------------------------------------------------------------------------------------------
    //! @brief      解放処理を行います.
    //---------------------------------------------------------------------------------------------
    void Release();

protected:
    //=============================================================================================
    // protected variables.
    //=============================================================================================
    /* NOTHING */

    //=============================================================================================
    // protected methods
    //=============================================================================================
    /* NOTHING */

private:
    //=============================================================================================
    // private variables.
    //=============================================================================================
    RefPtr<ID3D11Texture2D>            m_pTexture;     //!< テクスチャです.
    RefPtr<ID3D11ShaderResourceView>   m_pSRV;         //!< シェーダリソースビューです.
    RefPtr<ID3D11DepthStencilView>     m_pDSV;         //!< 深度ステンシルビューです.
    TargetDesc2D                       m_Desc;         //!< 構成設定です.

    //=============================================================================================
    // private methods.
    //=============================================================================================
    /* NOTHING */
};


} // namespace asdx

#endif//__ASDX_TARGET_H__
