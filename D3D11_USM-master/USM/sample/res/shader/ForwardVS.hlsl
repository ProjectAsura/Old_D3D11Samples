//-----------------------------------------------------------------------------------------
// File : ForwardVS.hlsl
// Desc : Forward Shading.
// Copyright(c) Project Asura. All right reserved.
//-----------------------------------------------------------------------------------------


///////////////////////////////////////////////////////////////////////////////////////////
// VSInput structure
///////////////////////////////////////////////////////////////////////////////////////////
struct VSInput
{
    float3  Position    : POSITION;         //!< 位置座標です(ローカル座標系).
    float3  Normal      : NORMAL;           //!< 法線ベクトルです(ローカル座標系).
    float3  Tangent     : TANGENT;          //!< 接ベクトルです(ローカル座標系).
    float2  TexCoord    : TEXCOORD;         //!< テクスチャ座標です.
};


///////////////////////////////////////////////////////////////////////////////////////////
// VSOutput structure
///////////////////////////////////////////////////////////////////////////////////////////
struct VSOutput
{
    float4  Position    : SV_POSITION;
    float4  WorldPos    : WORLD_POSITION;
    float3  Normal      : NORMAL;
    float2  TexCoord    : TEXCOORD0;
    float3  LightDir    : LIGHT_DIRECTION;
    float3  ViewDir     : VIEW_DIRECTION;
    float4  SdwCoord    : SHADOW_COORD;
};


///////////////////////////////////////////////////////////////////////////////////////////
// CBMatrix buffer
///////////////////////////////////////////////////////////////////////////////////////////
cbuffer CBMatrix : register( b1 )
{
    float4x4 World      : packoffset( c0 );     //!< ワールド行列です.
    float4x4 View       : packoffset( c4 );     //!< ビュー行列です.
    float4x4 Proj       : packoffset( c8 );     //!< 射影行列です.
    float4   CameraPos  : packoffset( c12 );    //!< カメラ位置です.
    float4   LightDir   : packoffset( c13 );    //!< ライト位置です.
    float4x4 Shadow     : packoffset( c14 );    //!< シャドウマップ行列.
};


//-----------------------------------------------------------------------------------------
//! @brief      頂点シェーダのメインエントリーポイントです.
//-----------------------------------------------------------------------------------------
VSOutput VSFunc( VSInput input )
{
    VSOutput output = (VSOutput)0;

    float4 localPos = float4( input.Position, 1.0f );
    float4 worldPos = mul( World, localPos );
    float4 viewPos  = mul( View, worldPos );
    float4 projPos  = mul( Proj, viewPos );

    output.Position = projPos;
    output.WorldPos = worldPos;
    output.LightDir = -LightDir.xyz;
    output.ViewDir  = worldPos.xyz - CameraPos.xyz;

    output.Normal   = input.Normal;
    output.TexCoord = input.TexCoord;

    output.SdwCoord = mul( Shadow, worldPos );

    return output;
}