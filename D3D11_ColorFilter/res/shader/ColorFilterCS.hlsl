//-----------------------------------------------------------------------------
// File : ColorFilterCS.hlsl
// Desc : Color Filter.
// Copyright(c) Project Asura. All right reserved.
//-----------------------------------------------------------------------------

// スレッドサイズ.
#define THREAD_SIZE (8)

// Shader Model 5系かどうか?
#define IS_SM5 (1)

///////////////////////////////////////////////////////////////////////////////
// ColorFilterParam structure
///////////////////////////////////////////////////////////////////////////////
cbuffer CbColorFilter : register(b0)
{
    uint2       DipsatchArgs : packoffset(c0);   // Dispatch()メソッドに渡した引数.
    float4x4    ColorMatrix  : packoffset(c1);   // カラー変換行列.
};

//-----------------------------------------------------------------------------
// Resources.
//-----------------------------------------------------------------------------
Texture2D<float4>   Input   : register(t0);
RWTexture2D<float4> Output  : register(u0);


//-----------------------------------------------------------------------------
//! @brief      スレッドグループのタイリングを行う.
//!
//! @param[in]      dispatchGridDim     ID3D12GraphicsCommandList::Dipatch(X, Y, Z)で渡した(X, Y)の値.
//! @param[in]      groupId             グループID
//! @param[in]      groupTheradId       グループスレッドID.
//! @return     スレッドIDを返却する.
//-----------------------------------------------------------------------------
uint2 CalcSwizzledThreaId(uint2 dispatchDim, uint2 groupId, uint2 groupThreadId)
{
    // "CTA" (Cooperative Thread Array) == Thread Group in DirectX terminology
    const uint2 CTA_Dim = uint2(THREAD_SIZE, THREAD_SIZE);
    const uint N = 16; // 16 スレッドグループで起動.

    // 1タイル内のスレッドグループの総数.
    uint number_of_CTAs_in_a_perfect_tile = N * (dispatchDim.y);

    // 考えうる完全なタイルの数.
    uint number_of_perfect_tiles = dispatchDim.x / N;

    // 完全なタイルにおけるスレッドグループの総数.
    uint total_CTAs_in_all_perfect_tiles = number_of_perfect_tiles * N * dispatchDim.y - 1;
    uint threadGroupIDFlattened = dispatchDim.x * groupId.y + groupId.x;

    // 現在のスレッドグループからタイルIDへのマッピング.
    uint tile_ID_of_current_CTA = threadGroupIDFlattened / number_of_CTAs_in_a_perfect_tile;
    uint local_CTA_ID_within_current_tile = threadGroupIDFlattened % number_of_CTAs_in_a_perfect_tile;

    uint local_CTA_ID_y_within_current_tile = local_CTA_ID_within_current_tile / N;
    uint local_CTA_ID_x_within_current_tile = local_CTA_ID_within_current_tile % N;
 
    if(total_CTAs_in_all_perfect_tiles < threadGroupIDFlattened)
    {
        // 最後のタイルに不完全な次元があり、最後のタイルからのCTAが起動された場合にのみ実行されるパス.
        uint x_dimension_of_last_tile = dispatchDim.x % N;
    #if IS_SM5
        // SM5.0だとコンパイルエラーになるので対策.
        if (x_dimension_of_last_tile > 0)
        {
            local_CTA_ID_y_within_current_tile = local_CTA_ID_within_current_tile / x_dimension_of_last_tile;
            local_CTA_ID_x_within_current_tile = local_CTA_ID_within_current_tile % x_dimension_of_last_tile;
        }
    #else
        local_CTA_ID_y_within_current_tile = local_CTA_ID_within_current_tile / x_dimension_of_last_tile;
        local_CTA_ID_x_within_current_tile = local_CTA_ID_within_current_tile % x_dimension_of_last_tile;
    #endif
    }

    uint swizzledThreadGroupIDFlattened = tile_ID_of_current_CTA * N
      + local_CTA_ID_y_within_current_tile * dispatchDim.x
      + local_CTA_ID_x_within_current_tile;

    uint2 swizzledThreadGroupID;
    swizzledThreadGroupID.y = swizzledThreadGroupIDFlattened / dispatchDim.x;
    swizzledThreadGroupID.x = swizzledThreadGroupIDFlattened % dispatchDim.x;

    uint2 swizzledThreadID;
    swizzledThreadID.x = CTA_Dim.x * swizzledThreadGroupID.x + groupThreadId.x;
    swizzledThreadID.y = CTA_Dim.y * swizzledThreadGroupID.y + groupThreadId.y;

    return swizzledThreadID;
}


//-----------------------------------------------------------------------------
//      メインエントリーポイントです.
//-----------------------------------------------------------------------------
[numthreads(THREAD_SIZE, THREAD_SIZE, 1)]
void main
(
    uint3 groupId       : SV_GroupID,
    uint3 groupThreadId : SV_GroupThreadID
)
{
    uint2 id = CalcSwizzledThreaId(DipsatchArgs, groupId.xy, groupThreadId.xy);
    Output[id] = mul(ColorMatrix, Input[id]);
}