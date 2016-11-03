﻿//--------------------------------------------------------------------------------------------
// File : asdxMesh.h
// Desc : Resource Mesh Module.
// Copyright(c) Project Asura. All right reserved.
//--------------------------------------------------------------------------------------------

#ifndef __ASDX_RES_MESH_H__
#define __ASDX_RES_MESH_H__

//--------------------------------------------------------------------------------------------
// Includes
//--------------------------------------------------------------------------------------------
#include <asdxMath.h>
#include <d3d11.h>


namespace asdx {

//////////////////////////////////////////////////////////////////////////////////////////////
// ResMesh class
//////////////////////////////////////////////////////////////////////////////////////////////
class ResMesh
{
    //========================================================================================
    // list of friend classes and methods.
    //========================================================================================
    /* NOTHING */

public:
    //========================================================================================
    // Type definition.
    //========================================================================================
    typedef u32      Index;

    //========================================================================================
    // public variables.
    //========================================================================================
    static const u32 NUM_FILENAME        = 256;
    static const u32 NUM_INPUT_ELEMENT   = 4;
    static const D3D11_INPUT_ELEMENT_DESC INPUT_ELEMENTS[ NUM_INPUT_ELEMENT ];

    //////////////////////////////////////////////////////////////////////////////////////////
    // Vertex structure
    //////////////////////////////////////////////////////////////////////////////////////////
    struct Vertex
    {
        asdx::Vector3    Position;   //!< 位置座標です.
        asdx::Vector3    Normal;     //!< 法線ベクトルです.
        asdx::Vector3    Tangent;    //!< 接ベクトルです.
        asdx::Vector2    TexCoord;   //!< テクスチャ座標です.
    };

    //////////////////////////////////////////////////////////////////////////////////////////
    // Material structure
    //////////////////////////////////////////////////////////////////////////////////////////
    struct Material
    {
        asdx::Vector3   Ambient;                            //!< 環境色です.
        asdx::Vector3   Diffuse;                            //!< 拡散反射色です.
        asdx::Vector3   Specular;                           //!< 鏡面反射色です.
        asdx::Vector3   Emissive;                           //!< 自己照明色です.
        f32             Alpha;                              //!< 透過度です.
        f32             Power;                              //!< 鏡面反射強度です.
        char            AmbientMap     [ NUM_FILENAME ];    //!< アンビエントマップです.
        char            DiffuseMap     [ NUM_FILENAME ];    //!< ディフューズマップです.
        char            SpecularMap    [ NUM_FILENAME ];    //!< スペキュラーマップです.
        char            BumpMap        [ NUM_FILENAME ];    //!< 凹凸マップです.
        char            DisplacementMap[ NUM_FILENAME ];    //!< 変位マップです.
    };

    /////////////////////////////////////////////////////////////////////////////////////////
    // Subset structure
    /////////////////////////////////////////////////////////////////////////////////////////
    struct Subset
    {
        u32      IndexOffset;        //!< インデックバッファ先頭からのオフセットです.
        u32      IndexCount;         //!< 描画するインデックス数です.
        u32      MaterialID;         //!< マテリアルIDです.
    };


    //========================================================================================
    // public methods.
    //========================================================================================

    //----------------------------------------------------------------------------------------
    //! @brief      コンストラクタです.
    //----------------------------------------------------------------------------------------
    ResMesh();

    //----------------------------------------------------------------------------------------
    //! @brief      コピーコンストラクタです.
    //----------------------------------------------------------------------------------------
    ResMesh( const ResMesh& ResMesh );

    //----------------------------------------------------------------------------------------
    //! @brief      デストラクタです.
    //----------------------------------------------------------------------------------------
    virtual ~ResMesh();

    //----------------------------------------------------------------------------------------
    //! @brief      代入演算子です.
    //!
    //! @param [in]     ResMesh            代入する値です.
    //! @return     代入結果を返却します.
    //----------------------------------------------------------------------------------------
    ResMesh& operator = ( const ResMesh& ResMesh );

    //----------------------------------------------------------------------------------------
    //! @brief      ファイルからデータをロードします.
    //!
    //! @param [in]     filename        入力ファイル名です.
    //! @retval true    ロードに成功.
    //! @retval false   ロードに失敗.
    //----------------------------------------------------------------------------------------
    bool        LoadFromFile( const char* filename );

    //----------------------------------------------------------------------------------------
    //! @brief      メモリを解放します.
    //----------------------------------------------------------------------------------------
    void        Release();

    //----------------------------------------------------------------------------------------
    //! @brief      頂点数を取得します.
    //!
    //! @return     頂点数を返却します.
    //----------------------------------------------------------------------------------------
    u32  GetVertexCount  () const;

    //----------------------------------------------------------------------------------------
    //! @brief      インデックス数を取得します.
    //!
    //! @return     インデックス数を返却します.
    //----------------------------------------------------------------------------------------
    u32  GetIndexCount   () const;

    //----------------------------------------------------------------------------------------
    //! @brief      マテリアル数を取得します.
    //!
    //! @return     マテリアル数を返却します.
    //----------------------------------------------------------------------------------------
    u32  GetMaterialCount() const;

    //----------------------------------------------------------------------------------------
    //! @brief      サブセット数を取得します.
    //!
    //! @return     サブセット数を返却します.
    //----------------------------------------------------------------------------------------
    u32  GetSubsetCount  () const;

    //----------------------------------------------------------------------------------------
    //! @brief      頂点データを取得します.
    //!
    //! @param [in]     idx     頂点番号.
    //! @return     指定された頂点データを返却します.
    //----------------------------------------------------------------------------------------
    ResMesh::Vertex      GetVertex   ( const u32 idx ) const;

    //----------------------------------------------------------------------------------------
    //! @brief      インデックスデータを取得します.
    //! 
    //! @param [in]     idx     インデックス番号.
    //! @return     指定されたインデックスデータを返却します.
    //----------------------------------------------------------------------------------------
    ResMesh::Index       GetIndex    ( const u32 idx ) const;

    //----------------------------------------------------------------------------------------
    //! @brief      マテリアルデータを取得します.
    //!
    //! @param [in]     idx     マテリアル番号.
    //! @return     指定されたマテリアルデータを返却します.
    //----------------------------------------------------------------------------------------
    ResMesh::Material    GetMaterial  ( const u32 idx ) const;

    //----------------------------------------------------------------------------------------
    //! @brief      サブセットデータを取得します.
    //!
    //! @param [in]     idx     サブセット番号.
    //! @return     指定されたサブセットデータを返却します.
    //----------------------------------------------------------------------------------------
    ResMesh::Subset      GetSubset   ( const u32 idx ) const;

    //----------------------------------------------------------------------------------------
    //! @brief      頂点データを取得します.
    //!
    //! @return     頂点データを返却します.
    //----------------------------------------------------------------------------------------
    const ResMesh::Vertex*     GetVertices () const;

    //----------------------------------------------------------------------------------------
    //! @brief      頂点インデックスデータを取得します.
    //!
    //! @return     頂点インデックスデータを返却します.
    //----------------------------------------------------------------------------------------
    const ResMesh::Index*      GetIndices  () const;

    //----------------------------------------------------------------------------------------
    //! @brief      マテリアルデータを取得します.
    //!
    //! @return     マテリアルデータを返却します.
    //----------------------------------------------------------------------------------------
    const ResMesh::Material*   GetMaterials () const;

    //----------------------------------------------------------------------------------------
    //! @brief      サブセットデータを取得します.
    //!
    //! @return     サブセットデータを返却します.
    //----------------------------------------------------------------------------------------
    const ResMesh::Subset*     GetSubsets  () const;

protected:
    //========================================================================================
    // protected variables.
    //========================================================================================
    u32  m_VertexCount;      //!< 頂点数です.
    u32  m_IndexCount;       //!< 頂点インデックス数です.
    u32  m_MaterialCount;    //!< マテリアル数です.
    u32  m_SubsetCount;      //!< サブセット数です.

    ResMesh::Vertex*     m_pVertex;          //!< 頂点データです.
    ResMesh::Index*      m_pIndex;           //!< 頂点インデックスデータです.
    ResMesh::Material*   m_pMaterial;        //!< マテリアルデータです.
    ResMesh::Subset*     m_pSubset;          //!< サブセットデータです.

    //========================================================================================
    // protected methods.
    //========================================================================================
    /* NOTHING */

private:
    //========================================================================================
    // private variables.
    //========================================================================================
    /* NOTHING */

    //========================================================================================
    // private methods.
    //========================================================================================
    /* NOTHING */
};


} // namespace asdx


#endif//__ASDX_RES_MESH_H__
