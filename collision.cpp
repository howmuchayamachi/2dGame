/*==============================================================================

   コリジョン判定 [collision.cpp]
														 Author : Harada Ren
														 Date   : 2025/07/03
--------------------------------------------------------------------------------

==============================================================================*/
#include "collision.h"
#include "direct3d.h"
#include "texture.h"
#include "shader.h"
using namespace DirectX;
#include "map.h"
#include <algorithm>


static constexpr int NUM_VERTEX = 5000; // 頂点数


static ID3D11Buffer* g_pVertexBuffer = nullptr; // 頂点バッファ

static ID3D11Device* g_pDevice = nullptr;
static ID3D11DeviceContext* g_pContext = nullptr;

static int g_WhiteTex = -1;


// 頂点構造体
struct Vertex
{
	XMFLOAT3 position; // 頂点座標
	XMFLOAT4 color; //頂点カラー
	XMFLOAT2 texcoord;//テクスチャ座標(UV)
};


//円の当たり判定
bool Collision_IsOverlapCircle(const Circle& a, const Circle& b){
	//aを基準として距離を測る
	float x1 = b.center.x - a.center.x;
	float y1 = b.center.y - a.center.y;
	
	return (a.radius + b.radius) * (a.radius + b.radius) > (x1 * x1 + y1 * y1);
}

//四角の当たり判定
bool Collision_IsOverlapBox(const Box& a, const Box& b){
	float at = a.center.y - a.half_height; //top
	float ab = a.center.y + a.half_height; //bottom
	float al = a.center.x - a.half_width; //left
	float ar = a.center.x + a.half_width; //right

	float bt = b.center.y - b.half_height; //top
	float bb = b.center.y + b.half_height; //bottom
	float bl = b.center.x - b.half_width; //left
	float br = b.center.x + b.half_width; //right

	return al<br && ar>bl && at<bb && ab>bt;
}

bool Collision_IsOverlapCircleVSBox(const Box& box, const Circle& circle){
	// 円の中心に最も近い四角形上の点を求める
	float closest_x = std::max(box.center.x - box.half_width, std::min(circle.center.x, box.center.x + box.half_width));
	float closest_y = std::max(box.center.y - box.half_height, std::min(circle.center.y, box.center.y + box.half_height));

	// 最も近い点と円の中心との距離を計算
	float distance_x = circle.center.x - closest_x;
	float distance_y = circle.center.y - closest_y;
	float distance_sq = (distance_x * distance_x) + (distance_y * distance_y); // 距離の2乗

	// 距離の2乗が円の半径の2乗より小さければ衝突している
	return distance_sq < (circle.radius * circle.radius);
}

bool Collision_IsOverlapOBBVSCircle(const OBB& obb, const Circle& circle)
{
	// 円の中心をOBBのローカル座標系に変換する
	XMVECTOR obb_center = XMLoadFloat2(&obb.center);
	XMVECTOR circle_center = XMLoadFloat2(&circle.center);
	XMVECTOR vec_to_circle = circle_center - obb_center; // OBBの中心から円の中心へのベクトル

	XMVECTOR obb_axis0 = XMLoadFloat2(&obb.axis[0]);
	XMVECTOR obb_axis1 = XMLoadFloat2(&obb.axis[1]);

	// ドット積を使って、OBBのローカル座標系での円の中心座標を計算
	XMFLOAT2 circle_center_local;
	circle_center_local.x = XMVectorGetX(XMVector2Dot(vec_to_circle, obb_axis0));
	circle_center_local.y = XMVectorGetY(XMVector2Dot(vec_to_circle, obb_axis1));


	// OBBの辺上で円の中心に最も近い点を求める
	float closest_x = std::max(-obb.half_extent.x, std::min(circle_center_local.x, obb.half_extent.x));
	float closest_y = std::max(-obb.half_extent.y, std::min(circle_center_local.y, obb.half_extent.y));


	//最も近い点と円の中心との距離を計算
	float distance_x = circle_center_local.x - closest_x;
	float distance_y = circle_center_local.y - closest_y;
	float distance_sq = (distance_x * distance_x) + (distance_y * distance_y); // 距離の2乗


	// 距離の2乗が円の半径の2乗より小さければ衝突している
	return distance_sq < (circle.radius * circle.radius);
}

bool Collision_IsOverlapOBBVSBox(const OBB& obb, const Box& box){
	//BoxをOBBとみなしてOBBどうしの当たり判定を見る
	//BoxをOBBに変換する
	OBB box_as_obb;
	box_as_obb.center = box.center;
	box_as_obb.half_extent = { box.half_width, box.half_height };
	//回転していないので軸ベクトルは標準の(1,0)と(0,1)
	box_as_obb.axis[0] = { 1.0f, 0.0f };
	box_as_obb.axis[1] = { 0.0f, 1.0f };

	//OBB vs OBB の当たり判定関数を呼び出して、その結果を返す
	return Collision_IsOverlapOBB(obb, box_as_obb);
}

bool Collision_IsOverlapOBB(const OBB& a, const OBB& b){
	//チェックする必要がある4つの軸（お互いのX軸とY軸）
	DirectX::XMFLOAT2 axes[4] = {
		a.axis[0], a.axis[1],
		b.axis[0], b.axis[1]
	};

	for (int i = 0; i < 4; ++i) {
		float a_min, a_max, b_min, b_max;
		//各軸に対してそれぞれのOBBの影の範囲を計算
		ProjectOBB(&a_min, &a_max, a, axes[i]);
		ProjectOBB(&b_min, &b_max, b, axes[i]);

		//影が完全に離れている（重なっていない）軸が一つでも見つかれば衝突していない
		if (a_max < b_min || b_max < a_min) {
			return false;
		}
	}

	// 4つの軸すべてで影が重なっていた場合衝突している
	return true;
}

static void ProjectOBB(float* min, float* max, const OBB& obb, const DirectX::XMFLOAT2& axis){
	//OBBの中心点を軸に射影する
	float p = obb.center.x * axis.x + obb.center.y * axis.y;

	//中心点から影の端までの最大距離を求める
	//OBBの2つの軸ベクトルを、判定軸にそれぞれ射影し、半分の長さを掛ける
	float r = obb.half_extent.x * (float)fabs(obb.axis[0].x * axis.x + obb.axis[0].y * axis.y) +
		obb.half_extent.y * (float)fabs(obb.axis[1].x * axis.x + obb.axis[1].y * axis.y);

	//中心の射影から、影の端までの距離を足し引きして、影の最小値と最大値を求める
	*min = p - r;
	*max = p + r;
}

void Collision_DebugInitialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext){
	//デバイスとデバイスコンテキストの保存
	g_pDevice = pDevice;
	g_pContext = pContext;
	
	//頂点バッファ生成
	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(Vertex) * NUM_VERTEX;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	g_pDevice->CreateBuffer(&bd, NULL, &g_pVertexBuffer);

	g_WhiteTex = Texture_Load(L"resource/texture/white.png");
}

void Collision_DebugFinalize(){
	SAFE_RELEASE(g_pVertexBuffer);
}

void Collision_DebugDraw(const Circle& circle, const DirectX::XMFLOAT4& color){
	//点の数を算出
	int NumVertex = (int)(circle.radius * 2.0f + XM_PI + 1);

	//シェーダーを描画パイプラインに設定
	Shader_Begin();

	//カメラのオフセットを取得
	XMFLOAT2 camera_offset = Map_GetWorldOffset();

	//ワールド座標であるboxの中心から、カメラの座標を引いてスクリーン座標に変換
	float screen_x = circle.center.x - camera_offset.x;
	float screen_y = circle.center.y - camera_offset.y;

	//頂点バッファをロックする
	D3D11_MAPPED_SUBRESOURCE msr;
	g_pContext->Map(g_pVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

	//頂点バッファへの仮想ポインタを取得
	Vertex* v = (Vertex*)msr.pData;

	const float rad = XM_2PI / NumVertex;

	for (int i = 0;i < NumVertex; i++) {
		v[i].position.x = cosf(rad * i) * circle.radius + screen_x;
		v[i].position.y = sinf(rad * i) * circle.radius + screen_y;
		v[i].position.z = 0.0f;
		v[i].color = color;
		v[i].texcoord = { 0.0f,0.0f };
	}


	//頂点バッファのロックを解除
	g_pContext->Unmap(g_pVertexBuffer, 0);

	//ワールドマトリクスにセット
	Shader_SetWorldMatrix(XMMatrixIdentity()); //単位行列

	//頂点バッファを描画パイプラインに設定
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	g_pContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &stride, &offset);

	//プリミティブトポロジ設定
	g_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	//テクスチャセット
	Texture_SetTexture(g_WhiteTex);

	//ポリゴン描画命令発行
	g_pContext->Draw(NumVertex, 0);

}

void Collision_DebugDraw(const Box& box, const DirectX::XMFLOAT4& color){
	//シェーダーを描画パイプラインに設定
	Shader_Begin();

	//カメラのオフセットを取得
	XMFLOAT2 camera_offset = Map_GetWorldOffset();

	//ワールド座標であるboxの中心から、カメラの座標を引いてスクリーン座標に変換
	float screen_x = box.center.x - camera_offset.x;
	float screen_y = box.center.y - camera_offset.y;

	//頂点バッファをロックする
	D3D11_MAPPED_SUBRESOURCE msr;
	g_pContext->Map(g_pVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

	//頂点バッファへの仮想ポインタを取得
	Vertex* v = (Vertex*)msr.pData;

	v[0].position = { screen_x - box.half_width , screen_y - box.half_height, 0.0f };
	v[1].position = { screen_x + box.half_width , screen_y - box.half_height, 0.0f };
	v[2].position = { screen_x + box.half_width , screen_y + box.half_height, 0.0f };
	v[3].position = { screen_x - box.half_width , screen_y + box.half_height, 0.0f };
	v[4].position = { screen_x - box.half_width , screen_y - box.half_height, 0.0f };


	for (int i = 0;i < 5;i++) {
		v[i].color = color;
		v[i].texcoord = { 0.0f,0.0f };
	}


	//頂点バッファのロックを解除
	g_pContext->Unmap(g_pVertexBuffer, 0);

	//ワールドマトリクスにセット
	Shader_SetWorldMatrix(XMMatrixIdentity()); //単位行列

	//頂点バッファを描画パイプラインに設定
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	g_pContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &stride, &offset);

	//プリミティブトポロジ設定
	g_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);

	//テクスチャセット
	Texture_SetTexture(g_WhiteTex);

	//ポリゴン描画命令発行
	g_pContext->Draw(5, 0);
}

void Collision_DebugDraw(const OBB& obb, const DirectX::XMFLOAT4& color) {
	//OBBの半分のサイズと軸ベクトルをXMVECTORにロード
	XMVECTOR half_extent_x = XMLoadFloat2(&obb.axis[0]) * obb.half_extent.x;
	XMVECTOR half_extent_y = XMLoadFloat2(&obb.axis[1]) * obb.half_extent.y;
	XMVECTOR center = XMLoadFloat2(&obb.center);

	//OBBの4つの頂点のワールド座標を計算
	XMFLOAT2 corners[4];
	XMStoreFloat2(&corners[0], center - half_extent_x - half_extent_y); // 左上
	XMStoreFloat2(&corners[1], center + half_extent_x - half_extent_y); // 右上
	XMStoreFloat2(&corners[2], center + half_extent_x + half_extent_y); // 右下
	XMStoreFloat2(&corners[3], center - half_extent_x + half_extent_y); // 左下

	//カメラのオフセットを取得して、各頂点をスクリーン座標に変換
	XMFLOAT2 Offset = Map_GetWorldOffset();
	for (int i = 0; i < 4; ++i) {
		corners[i].x -= Offset.x;
		corners[i].y -= Offset.y;
	}

	//描画のための頂点バッファを準備
	static constexpr int NumVertex = 5;
	D3D11_MAPPED_SUBRESOURCE msr;
	g_pContext->Map(g_pVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
	Vertex* v = (Vertex*)msr.pData;

	//4つの頂点を線で結ぶように設定 (最後は始点に戻る)
	v[0].position = { corners[0].x, corners[0].y, 0.0f };
	v[1].position = { corners[1].x, corners[1].y, 0.0f };
	v[2].position = { corners[2].x, corners[2].y, 0.0f };
	v[3].position = { corners[3].x, corners[3].y, 0.0f };
	v[4].position = { corners[0].x, corners[0].y, 0.0f }; // 始点に戻る

	for (int i = 0; i < NumVertex; ++i) {
		v[i].color = color;
		v[i].texcoord = { 0.0f, 0.0f };
	}
	g_pContext->Unmap(g_pVertexBuffer, 0);

	//描画パイプラインの設定と描画命令
	Shader_Begin();
	Shader_SetWorldMatrix(XMMatrixIdentity());
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	g_pContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &stride, &offset);
	g_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP); // 線を描画するモード
	Texture_SetTexture(-1); // テクスチャは使わない
	g_pContext->Draw(NumVertex, 0);
}