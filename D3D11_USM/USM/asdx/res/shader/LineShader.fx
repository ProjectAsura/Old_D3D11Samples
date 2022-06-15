//-------------------------------------------------------------------------------------------
// File : LineShader.fx
// Desc : Line Shader
// Copyright(c) Project Asura. All right reserved.
//-------------------------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////////////////////
// VSInput structure
/////////////////////////////////////////////////////////////////////////////////////////////
struct VSInput
{
    float3  Position : POSITION;    //!< 位置座標です.
    float4  Color    : VTXCOLOR;    //!< 頂点カラーです.
};

/////////////////////////////////////////////////////////////////////////////////////////////
// VSOutput structure
/////////////////////////////////////////////////////////////////////////////////////////////
struct VSOutput
{
    float4  Position : SV_POSITION; //!< 位置座標です.
    float4  Color    : COLOR0;      //!< カラー0です.
};

/////////////////////////////////////////////////////////////////////////////////////////////
// CbPerOnceVS constant buffer
/////////////////////////////////////////////////////////////////////////////////////////////
cbuffer CbPerOnceVS : register( b0 )
{
    float4x4 View;      //!< ビュー行列です.
    float4x4 Proj;      //!< 射影行列.
};

/////////////////////////////////////////////////////////////////////////////////////////////
// CbPerObjVS constant buffer
/////////////////////////////////////////////////////////////////////////////////////////////
cbuffer CbPerObjVS : register( b1 )
{
    float4x4 World;     //!< ワールド行列です.
}

//-------------------------------------------------------------------------------------------
//! @brief      頂点シェーダメインエントリーポイントです.
//-------------------------------------------------------------------------------------------
VSOutput VSFunc( VSInput input )
{
    VSOutput output = (VSOutput)0;

    // 位置座標を変換.
    float4 objPos = float4( input.Position, 1.0f );
    float4 worldPos = mul( World, objPos );
    float4 viewPos  = mul( View, worldPos );
    float4 projPos  = mul( Proj, viewPos );

    // 出力値書き込み
    output.Position = projPos;
    output.Color    = input.Color;

    return output;
}

//--------------------------------------------------------------------------------------------
//! @brief      ピクセルシェーダメインエントリーポイントです.
//--------------------------------------------------------------------------------------------
float4 PSFunc( VSOutput input ) : SV_TARGET0
{
    // 頂点カラーをそのまま出力.
    return input.Color;
}


