/*==============================================================================

   2D描画用頂点シェーダー [shader_vertex_2d.hlsl]
														 Author : Harada Ren
														 Date   : 2025/05/15
--------------------------------------------------------------------------------

==============================================================================*/

// 定数バッファ
cbuffer VS_CONSTANT_BUFFER : register(b0)
{
    float4x4 proj; //4*4行列
};

cbuffer VS_CONSTANT_BUFFER : register(b1)
{
    float4x4 world; //4*4行列
};

   

struct VS_IN{
    //:~ セマンティクス
    float4 posL : POSITION0; //System Value SV_Position
    float4 color: COLOR0; 
    float2 uv   : TEXCOORD0; 
};

struct VS_OUT{
    float4 posH : SV_POSITION; //System Value SV_Position
    float4 color: COLOR0;
    float2 uv   : TEXCOORD0; 
};

//=============================================================================
// 頂点シェーダ
//=============================================================================
VS_OUT main(VS_IN vi)
{
    VS_OUT vo;
    
    //座標変換
    float4x4 mtx = mul(world, proj);
    vo.posH = mul(vi.posL, mtx); 
    
    vo.color = vi.color;
    vo.uv = vi.uv;
    
	return vo;
}
