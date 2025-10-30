/*==============================================================================

   コリジョン判定 [collision.h]
														 Author : Harada Ren
														 Date   : 2025/07/03
--------------------------------------------------------------------------------

==============================================================================*/
#ifndef COLLISION_H
#define COLLISION_H

#include <d3d11.h>
#include <DirectXMath.h>


struct Circle {
	DirectX::XMFLOAT2 center; //中心座標
	float radius; //半径
};

struct Box {
	DirectX::XMFLOAT2 center; //中心座標
	float half_width; //半分の幅
	float half_height; //半分の高さ
};

//回転するBox
struct OBB {
	DirectX::XMFLOAT2 center; //中心座標
	DirectX::XMFLOAT2 half_extent; //各軸方向の半分の長さ
	DirectX::XMFLOAT2 axis[2]; //回転を表す2つの軸ベクトル
};

//円どうしの当たり判定
bool Collision_IsOverlapCircle(const Circle& a, const Circle& b);
//四角どうしの当たり判定
bool Collision_IsOverlapBox(const Box& a, const Box& b);
//円と四角のあたり判定
bool Collision_IsOverlapCircleVSBox(const Box& box, const Circle& circle);
// OBBと円の当たり判定
bool Collision_IsOverlapOBBVSCircle(const OBB& obb, const Circle& circle);
//OBBと四角の当たり判定
bool Collision_IsOverlapOBBVSBox(const OBB& obb, const Box& box);
//OBBどうしの当たり判定
bool Collision_IsOverlapOBB(const OBB& a, const OBB& b);

//分離軸の定理で使用する
static void ProjectOBB(float* min, float* max, const OBB& obb, const DirectX::XMFLOAT2& axis);

//デバッグ表示
void Collision_DebugInitialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
void Collision_DebugFinalize();
void Collision_DebugDraw(const Circle& circle, const DirectX::XMFLOAT4& color = { 1.0f,1.0f,1.0f,1.0f });
void Collision_DebugDraw(const Box& box, const DirectX::XMFLOAT4& color = { 1.0f,1.0f,1.0f,1.0f });
void Collision_DebugDraw(const OBB& obb, const DirectX::XMFLOAT4& color = { 1.0f,1.0f,1.0f,1.0f });



#endif //COLLISION_H