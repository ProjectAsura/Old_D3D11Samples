//-----------------------------------------------------------------------------------------
// File : ForwardPS.hlsl
// Desc : Forward Shading.
// Copyright(c) Project Asura. All right reserved.
//-----------------------------------------------------------------------------------------


// 円周率です.
#define PI          3.1415926535f

///////////////////////////////////////////////////////////////////////////////////////////
// VSOutput structure
///////////////////////////////////////////////////////////////////////////////////////////
struct VSOutput
{
    float4  Position : SV_POSITION;
    float4  WorldPos : WORLD_POSITION;
    float3  Normal   : NORMAL;
    float2  TexCoord : TEXCOORD0;
    float3  LightDir : LIGHT_DIRECTION;
    float3  ViewDir  : VIEW_DIRECTION;
    float4  SdwCoord : SHADOW_COORD;
};


///////////////////////////////////////////////////////////////////////////////////////////
// PSOutput structure
///////////////////////////////////////////////////////////////////////////////////////////
struct PSOutput
{
    float4 Color : SV_TARGET0;
};


/////////////////////////////////////////////////////////////////////////////////////////
// Material Buffer
/////////////////////////////////////////////////////////////////////////////////////////
cbuffer Material : register( b0 )
{
    float3 Diffuse  : packoffset( c0 );     //!< 拡散反射色です.
    float  Alpha    : packoffset( c0.w );   //!< 透過度です.
    float3 Specular : packoffset( c1 );     //!< 鏡面反射色です.
    float  Power    : packoffset( c1.w );   //!< 鏡面反射強度です.
    float3 Emissive : packoffset( c2 );     //!< 自己照明色です.
    float  Bump     : packoffset( c2.w );   //!< バンプマッピングフラグ.
};


//---------------------------------------------------------------------------------------
// Textures
//---------------------------------------------------------------------------------------
Texture2D       DiffuseMap    : register( t0 );     //!< ディフューズマップです.
Texture2D       SpecularMap   : register( t1 );     //!< スペキュラーマップです.
Texture2D       BumpMap       : register( t2 );     //!< バンプマップです.
Texture2D       ShadowMap     : register( t3 );     //!< シャドウマップです.


//---------------------------------------------------------------------------------------
// Samplers
//---------------------------------------------------------------------------------------
SamplerState            DiffuseSmp    : register( s0 );     //!< ディフューズマップ用サンプラーステートです.
SamplerState            SpecularSmp   : register( s1 );     //!< スペキュラーマップ用サンプラーステートです.
SamplerState            BumpSmp       : register( s2 );     //!< バンプマップ用サンプラーステートです.
SamplerComparisonState  ShadowSmp     : register( s3 );     //!< シャドウマップ用サンプラー比較ステートです.


//-----------------------------------------------------------------------------------------------
//! @biref      正規化ランバートライティングを計算します.
//!
//! @param [in]     diffuse     拡散反射色.
//! @param [in]     lightDir    ライトベクトル.
//! @param [in]     normal      法線ベクトル.
//! @return     ランバートライティングした結果を返却します.
//-----------------------------------------------------------------------------------------------
float3 NormalizedLambert( float3 diffuse, float3 lightDir, float3 normal )
{
   return diffuse * max( dot( normal, lightDir ), 0.0f ) * ( 1.0f / PI );
}

//------------------------------------------------------------------------------------------------
//! @brief      正規化フォンライティングを行います.
//!
//! @param [in]     specular    鏡面反射色.
//! @param [in]     power       鏡面反射強度.
//! @param [in]     viewDir     視線ベクトル.
//! @param [in]     normal      法線ベクトル.
//! @parma [in]     lightDir    ライトベクトル.
//! @return     フォンライティングの結果を返却します.
//------------------------------------------------------------------------------------------------
float3 NormalizedPhong( float3 specular, float power, float3 viewDir, float3 normal, float3 lightDir )
{
    float3 R = -viewDir + ( 2.0f * dot( normal, viewDir ) * normal );

    return specular * pow( max ( dot( lightDir, R ), 0.0f ), power ) * ( ( power + 1.0f )/ ( 2.0 * PI ) );
}


//------------------------------------------------------------------------------------------------
//! @brief      ピクセルシェーダのエントリーポイントです.
//------------------------------------------------------------------------------------------------
PSOutput PSFunc( VSOutput input )
{
    PSOutput output = (PSOutput)0;

    // ディフューズマップをフェッチ.
    float4 mapKd = DiffuseMap.Sample( DiffuseSmp, input.TexCoord );

    // アルファテスト.
    clip( ( mapKd.a < 0.125f ) ? -1.0f : 1.0f );

    // スペキュラーマップをフェッチ.
    float4 mapKs = SpecularMap.Sample( SpecularSmp, input.TexCoord );

    // シャドウマップの深度値と比較する.
    float3 shadowCoord = input.SdwCoord.xyz / input.SdwCoord.w;

    // 最大深度傾斜を求める.
    float  maxDepthSlope = max( abs( ddx( shadowCoord.z ) ), abs( ddy( shadowCoord.z ) ) );

    float  shadowThreshold = 1.0f;      // シャドウにするかどうかの閾値です.
    float  bias            = 0.01f;     // 固定バイアスです.
    float  slopeScaledBias = 0.01f;     // 深度傾斜.
    float  depthBiasClamp  = 0.1f;      // バイアスクランプ値.

    float  shadowBias = bias + slopeScaledBias * maxDepthSlope;
    shadowBias = min( shadowBias, depthBiasClamp );

    float3 shadowColor     = float3( 0.25f, 0.25f, 0.25f );
    shadowThreshold = ShadowMap.SampleCmpLevelZero( ShadowSmp, shadowCoord.xy, shadowCoord.z - shadowBias );
    shadowColor     = lerp( shadowColor, float3( 1.0f, 1.0f, 1.0f ), shadowThreshold );

    // スペキュラーマップをフェッチ.
    float4 spe = SpecularMap.Sample( SpecularSmp, input.TexCoord );
    {
        float3 N = normalize( input.Normal );
        float3 V = normalize( input.ViewDir );
        float3 L = normalize( input.LightDir );

        float3 diffuse   = NormalizedLambert( Diffuse * mapKd.rgb, L, N );
        float3 specular  = NormalizedPhong( Specular * mapKs.rgb, Power, V, N, L );
        output.Color.rgb = ( diffuse + specular ) * shadowColor;
        output.Color.a   = Alpha;
    }

    return output;
}