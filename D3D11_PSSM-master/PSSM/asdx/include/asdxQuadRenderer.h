﻿//--------------------------------------------------------------------------------------------
// File : asdxQuadRenderer.h
// Desc : Asura Quad Renderer Module.
// Copyright(c) Project Asura. All right reserved.
//--------------------------------------------------------------------------------------------

#ifndef __ASDX_QUAD_RENDERER_H__
#define __ASDX_QUAD_RENDERER_H__


//---------------------------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------------------------
#include <d3d11.h>
#include <asdxMath.h>


namespace asdx {

///////////////////////////////////////////////////////////////////////////////////////////////
// QuadRenderer class
///////////////////////////////////////////////////////////////////////////////////////////////
class QuadRenderer
{
    //=========================================================================================
    // list of friend classes and methods.
    //=========================================================================================
    /* NOTHING */

private:
    //=========================================================================================
    // private variables.
    //=========================================================================================
    /* NOTHING */

    //=========================================================================================
    // private methods.
    //=========================================================================================
    QuadRenderer    ( const QuadRenderer& value );      // アクセス禁止.
    void operator = ( const QuadRenderer& value );      // アクセス禁止.

protected:
    //=========================================================================================
    // protected varaibles.
    //=========================================================================================
    ID3D11Buffer*       m_pVB;              //!< 頂点バッファです.
    ID3D11InputLayout*  m_pIL;              //!< 入力レイアウトです.
    u32                 m_Stride;           //!< ストライドです.
    u32                 m_Offset;           //!< オフセットです.

    //=========================================================================================
    // protected methods.
    //=========================================================================================
    /* NOTHING */

public:
    ///////////////////////////////////////////////////////////////////////////////////////////
    // Vertex structure
    //! @brief      頂点データ構造体です.
    ///////////////////////////////////////////////////////////////////////////////////////////
    struct Vertex
    {
        asdx::Vector3    Position;       //!< 位置座標です.
        asdx::Vector2    TexCoord;       //!< テクスチャ座標です.
    };

    //=========================================================================================
    // public variables.
    //=========================================================================================
    static const u32                        NUM_INPUT_ELEMENT = 2;                      //!< 入力要素数です.
    static const D3D11_INPUT_ELEMENT_DESC   INPUT_ELEMENTS[ NUM_INPUT_ELEMENT ];        //!< 入力要素です.

    //=========================================================================================
    // public methods.
    //=========================================================================================

    //-----------------------------------------------------------------------------------------
    //! @brief      コンストラクタです.
    //-----------------------------------------------------------------------------------------
    QuadRenderer();

    //-----------------------------------------------------------------------------------------
    //! @brief      デストラクタです.
    //-----------------------------------------------------------------------------------------
    virtual ~QuadRenderer();

    //-----------------------------------------------------------------------------------------
    //! @brief      初期化処理を行います.
    //!
    //! @param [in]     pDevice             デバイスです.
    //! @param [in]     pShaderBytecode     シェーダのバイトコードです.
    //! @param [in]     byteCodeLength      シェーダのバイトコードの長さです.
    //! @retval true    初期化に成功.
    //! @retval false   初期化に失敗.
    //-----------------------------------------------------------------------------------------
    bool Init( 
        ID3D11Device* pDevice,
        const void*   pShaderBytecode,
        const u32     byteCodeLength );

    //-----------------------------------------------------------------------------------------
    //! @brief      終了処理を行います.
    //-----------------------------------------------------------------------------------------
    void Term();

    //-----------------------------------------------------------------------------------------
    //! @brief      描画処理を行います.
    //!
    //! @param [in]     pDeviceContext      デバイスコンテキストです.
    //! @note       この関数は内部で，頂点バッファと入力レイアウトの設定を行い, 描画キックのみを行います.
    //!             シェーダやレンダーステートの設定等はアプリケーション側で設定する必要があります.
    //-----------------------------------------------------------------------------------------
    void Draw( ID3D11DeviceContext* pDeviceContext );
};

} // namespace asdx

#endif//__ASDX_QUAD_RENDERER_H__
