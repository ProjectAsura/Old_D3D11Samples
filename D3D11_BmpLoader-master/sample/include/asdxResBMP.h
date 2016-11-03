//-------------------------------------------------------------------------------------------------
// File : a3dResBMP.h
// Desc : Bitmap Module.
// Copyright(c) Project Asura. All right reserved.
//-------------------------------------------------------------------------------------------------

#ifndef __ASDX_RES_BMP_H__
#define __ASDX_RES_BMP_H__

//-------------------------------------------------------------------------------------------------
// Includes
//-------------------------------------------------------------------------------------------------
#include <asdxTypedef.h>
#include <asdxILoadable.h>

namespace asdx {

///////////////////////////////////////////////////////////////////////////////////////////////////
// BMP_COMPRESSION_TYPE enum
///////////////////////////////////////////////////////////////////////////////////////////////////
enum BMP_COMPRESSION_TYPE
{
    BMP_COMPRESSION_RGB       = 0,      // 無圧縮.
    BMP_COMPRESSION_RLE8      = 1,      // RLE圧縮 8 bits/pixel.
    BMP_COMPRESSION_RLE4      = 2,      // RLE圧縮 4 bits/pixel.
    BMP_COMPRESSION_BITFIELDS = 3,      // ビットフィールド.
};

///////////////////////////////////////////////////////////////////////////////////////////////////
// BMP_COLOR_SPACE_TYPE enum
///////////////////////////////////////////////////////////////////////////////////////////////////
enum BMP_COLOR_SPACE_TYPE
{
    BMP_COLOR_SPACE_CALIBRATED_RGB  = 0x00000000,
    BMP_COLOR_SPACE_SRGB            = 0x73524742,
    BMP_COLOR_SPACE_WIN_COLOR_SPACE = 0x57696E20,
    BMP_COLOR_SPACE_LINK            = 0x4C494E4B,
    BMP_COLOR_SPACE_MBED            = 0x4D424544,
};

///////////////////////////////////////////////////////////////////////////////////////////////////
// BMP_FILE_HEADER structure
///////////////////////////////////////////////////////////////////////////////////////////////////
#pragma pack( push, 1 )
struct BMP_FILE_HEADER
{
    u16     Type;           // ファイルタイプ 'BM'
    u32     Size;           // ファイルサイズ.
    u16     Reserved1;      // 予約領域 (0固定).
    u16     Reserved2;      // 予約領域 (0固定).
    u32     OffBits;        // ファイル先頭から画像データまでのオフセット.
};
#pragma pack( pop )

///////////////////////////////////////////////////////////////////////////////////////////////////
// BMP_INFO_HEADER structure
///////////////////////////////////////////////////////////////////////////////////////////////////
#pragma pack( push, 1 )
struct BMP_INFO_HEADER
{
    u32     Size;           // ヘッダサイズ (40固定).
    s32     Width;          // 画像の横幅.
    s32     Height;         // 画像の縦幅.
    u16     Planes;         // プレーン数 (1固定).
    u16     BitCount;       // 1ピクセルあたりのビット数.
    u32     Compression;    // 圧縮形式.
    u32     SizeImage;      // 画像データ部のサイズ.
    s32     ResolutionX;    // 横方向の解像度.
    s32     ResolutionY;    // 縦方向の解像度.
    u32     ClrUsed;        // 格納されているパレット数.
    u32     ClrImportant;   // 重要なパレットのインデックス.
};
#pragma pack( pop )

///////////////////////////////////////////////////////////////////////////////////////////////////
// BMP_HEADER_V4 structure
///////////////////////////////////////////////////////////////////////////////////////////////////
#pragma pack( push, 1 )
struct BMP_HEADER_V4
{
    u32     Size;               //!< ヘッダサイズ (108固定).
    s32     Width;              //!< 画像の横幅.
    s32     Height;             //!< 画像の縦幅.
    u16     Planes;             //!< プレーン数.
    u16     BitCount;           //!< 1ピクセルあたりのビット数.
    u32     Compression;        //!< 圧縮形式.
    u32     SizeImage;          //!< 画像データ部のサイズ.
    s32     ResolutionX;        //!< 横方向の解像度.
    s32     ResolutionY;        //!< 縦方向の解像度.
    u32     ClrUsed;            //!< 格納されているパレット数.
    u32     ClrImportant;       //!< 重要なパレットインデックス.

    u32     MaskR;              //!< 赤成分のカラーマスク.
    u32     MaskG;              //!< 緑成分のカラーマスク.
    u32     MaskB;              //!< 青成分のカラーマスク.
    u32     MaskA;              //!< α成分のカラーマスク.
    u32     ColorSpaceType;     //!< 色空間.
    s32     RedX;               //!< 赤成分に対するCIEXYZのX座標
    s32     RedY;               //!< 赤成分に対するCIEXYZのY座標
    s32     RedZ;               //!< 赤成分に対するCIEXYZのZ座標
    s32     GreenX;             //!< 緑成分に対するCIEXYZのX座標
    s32     GreenY;             //!< 緑成分に対するCIEXYZのY座標
    s32     GreenZ;             //!< 緑成分に対するCIEXYZのZ座標
    s32     BlueX;              //!< 青成分に対するCIEXYZのX座標
    s32     BlueY;              //!< 青成分に対するCIEXYZのY座標
    s32     BlueZ;              //!< 青成分に対するCIEXYZのZ座標
    u32     GammaR;             //!< 赤成分のガンマ値.
    u32     GammaG;             //!< 緑成分のガンマ値.
    u32     GammaB;             //!< 青成分のガンマ値.
};
#pragma pack( pop )

///////////////////////////////////////////////////////////////////////////////////////////////////
// BMP_HEADER_V5 structure
///////////////////////////////////////////////////////////////////////////////////////////////////
#pragma pack( push, 1 )
struct BMP_HEADER_V5
{
    u32     Size;               //!< ヘッダサイズ (124固定).
    s32     Width;              //!< 画像の横幅.
    s32     Height;             //!< 画像の縦幅.
    u16     Planes;             //!< プレーン数.
    u16     BitCount;           //!< 1ピクセルあたりのビット数.
    u32     Compression;        //!< 圧縮形式.
    u32     SizeImage;          //!< 画像データ部のサイズ.
    s32     ResolutionX;        //!< 横方向の解像度.
    s32     ResolutionY;        //!< 縦方向の解像度.
    u32     ClrUsed;            //!< 格納されているパレット数.
    u32     ClrImportant;       //!< 重要なパレットインデックス.

    u32     MaskR;              //!< 赤成分のカラーマスク.
    u32     MaskG;              //!< 緑成分のカラーマスク.
    u32     MaskB;              //!< 青成分のカラーマスク.
    u32     MaskA;              //!< α成分のカラーマスク.
    u32     ColorSpaceType;     //!< 色空間.
    s32     RedX;               //!< 赤成分に対するCIEXYZのX座標
    s32     RedY;               //!< 赤成分に対するCIEXYZのY座標
    s32     RedZ;               //!< 赤成分に対するCIEXYZのZ座標
    s32     GreenX;             //!< 緑成分に対するCIEXYZのX座標
    s32     GreenY;             //!< 緑成分に対するCIEXYZのY座標
    s32     GreenZ;             //!< 緑成分に対するCIEXYZのZ座標
    s32     BlueX;              //!< 青成分に対するCIEXYZのX座標
    s32     BlueY;              //!< 青成分に対するCIEXYZのY座標
    s32     BlueZ;              //!< 青成分に対するCIEXYZのZ座標
    u32     GammaR;             //!< 赤成分のガンマ値.
    u32     GammaG;             //!< 緑成分のガンマ値.
    u32     GammaB;             //!< 青成分のガンマ値.

    u32     Intent;             //!< レンダリング意図.
    u32     ProfileData;        //!< プロファイルデータのオフセット(単位バイト).
    u32     ProfileSize;        //!< プロファイルデータのサイズ'(単位バイト).
    u32     Reserved;           //!< 予約領域(常に0).
};
#pragma pack( pop )


///////////////////////////////////////////////////////////////////////////////////////////////////
// BMP_CORE_HEADER structure
///////////////////////////////////////////////////////////////////////////////////////////////////
struct BMP_CORE_HEADER
{
    u32     Size;
    u16     Width;
    u16     Height;
    u16     Planes;
    u16     BitCount;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
// ResBMP class
///////////////////////////////////////////////////////////////////////////////////////////////////
class ResBMP : public ILoadable
{
    //=============================================================================================
    // list of friend classes and methods.
    //=============================================================================================
    /* NOTHING */

public:
    ///////////////////////////////////////////////////////////////////////////////////////////////
    // Format
    ///////////////////////////////////////////////////////////////////////////////////////////////
    enum Format
    {
        Format_RGB = 0,     // RGB形式  3コンポーネント.
        Format_RGBA,        // RGBA形式 4コンポーネント.
        Format_RGB_SRGB,    // RGB形式  sRGB空間  
        Format_RGBA_SRGB,   // RGBA形式 sRGB空間.
    };

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
    ResBMP();

    //---------------------------------------------------------------------------------------------
    //! @brief      コピーコンストラクタです.
    //!
    //! @param[in]      value       コピー元の値です.
    //---------------------------------------------------------------------------------------------
    ResBMP( const ResBMP& value );

    //---------------------------------------------------------------------------------------------
    //! @brief      デストラクタです.
    //---------------------------------------------------------------------------------------------
    virtual ~ResBMP();

    //---------------------------------------------------------------------------------------------
    //! @brief      ファイルから読み込みします.
    //!
    //! @param[in]      filename        ファイル名.
    //! @retval true    読み込みに成功.
    //! @retval false   読み込みに失敗.
    //---------------------------------------------------------------------------------------------
    bool Load( const wchar_t* filename ) override;

    //---------------------------------------------------------------------------------------------
    //! @brief      メモリを解放します.
    //---------------------------------------------------------------------------------------------
    void Release();

    //---------------------------------------------------------------------------------------------
    //! @brief      画像の横幅を取得します.
    //!
    //! @return     画像の横幅を返却します.
    //---------------------------------------------------------------------------------------------
    const u32 GetWidth() const;

    //---------------------------------------------------------------------------------------------
    //! @brief      画像の縦幅を取得します.
    //!
    //! @return     画像の縦幅を返却します.
    //---------------------------------------------------------------------------------------------
    const u32 GetHeight() const;

    //---------------------------------------------------------------------------------------------
    //! @brief      フォーマットを取得します.
    //!
    //! @return     フォーマットを返却します.
    //---------------------------------------------------------------------------------------------
    const u32 GetFormat() const;

    //---------------------------------------------------------------------------------------------
    //! @brief      ピクセルデータを取得します.
    //!
    //! @return     ピクセルデータを返却します.
    //---------------------------------------------------------------------------------------------
    const u8* GetPixels() const;

    //---------------------------------------------------------------------------------------------
    //! @brief      代入演算子です.
    //!
    //! @param[in]      value       代入する値です.
    //! @return     代入結果を返却します.
    //---------------------------------------------------------------------------------------------
    ResBMP& operator = ( const ResBMP& value );

    //---------------------------------------------------------------------------------------------
    //! @brief      等価比較演算子です.
    //! 
    //! @param[in]      value       比較する値です.
    //! @retval true    等価です.
    //! @retval false   非等価です.
    //---------------------------------------------------------------------------------------------
    bool operator == ( const ResBMP& value ) const;

    //---------------------------------------------------------------------------------------------
    //! @brief      非等価比較演算子です.
    //!
    //! @param[in]      value       比較する値です.
    //! @retval true    非等価です.
    //! @retval false   等価です.
    //---------------------------------------------------------------------------------------------
    bool operator != ( const ResBMP& value ) const;

protected:
    //=============================================================================================
    // protected variables.
    //=============================================================================================
    /* NOTHING */

    //=============================================================================================
    // protected methods.
    //=============================================================================================
    /* NOTHING */

private:
    //=============================================================================================
    // private variables.
    //=============================================================================================
    u32     m_Width;        //!< 画像の横幅です.
    u32     m_Height;       //!< 画像の縦幅です.
    u32     m_Format;       //!< フォーマットです.
    u8*     m_pPixels;      //!< ピクセルデータです.
    u32     m_HashKey;      //!< ハッシュキーです.

    //=============================================================================================
    // private methods.
    //=============================================================================================
    /* NOTHING */
};


} // namespace a3d


#endif//__ASDX_RES_BMP_H__
