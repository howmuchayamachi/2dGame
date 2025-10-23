/*==============================================================================

   スプライト描画 [sprite.cpp]
														 Author : Harada Ren
														 Date   : 2025/05/15
--------------------------------------------------------------------------------

==============================================================================*/

#include <d3d11.h>
#include <DirectXMath.h>
using namespace DirectX;
#include "direct3d.h"
#include "shader.h"
#include "debug_ostream.h"
#include "sprite.h"
#include "texture.h"



static constexpr int NUM_VERTEX = 4; // 頂点数


static ID3D11Buffer* g_pVertexBuffer = nullptr; // 頂点バッファ
static ID3D11ShaderResourceView* g_pTexture = nullptr; //テクスチャ

static ID3D11Device* g_pDevice = nullptr;
static ID3D11DeviceContext* g_pContext = nullptr;


// 頂点構造体
struct Vertex {
	XMFLOAT3 position; // 頂点座標

	XMFLOAT4 color; //頂点カラー

	XMFLOAT2 texcoord;//テクスチャ座標(UV)
};


void Sprite_Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) {
	// デバイスとデバイスコンテキストのチェック
	if (!pDevice || !pContext) {
		hal::dout << "Sprite_Initialize() : 与えられたデバイスかコンテキストが不正です" << std::endl;
		return;
	}

	// デバイスとデバイスコンテキストの保存
	g_pDevice = pDevice;
	g_pContext = pContext;

	// 頂点バッファ生成
	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(Vertex) * NUM_VERTEX;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	g_pDevice->CreateBuffer(&bd, NULL, &g_pVertexBuffer);

}

void Sprite_Finalize(void) {
	SAFE_RELEASE(g_pTexture);
	SAFE_RELEASE(g_pVertexBuffer);
}

void Sprite_Begin() {
	// 頂点情報を書き込み
	const float SCREEN_WIDTH = (float)Direct3D_GetBackBufferWidth();
	const float SCREEN_HEIGHT = (float)Direct3D_GetBackBufferHeight();
	// 頂点シェーダーに変換行列を設定
	Shader_SetProjectionMatrix(XMMatrixOrthographicOffCenterLH(0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f, 0.0f, 1.0f));

}

//テクスチャ全表示
void Sprite_Draw(int texid, float dx, float dy, const XMFLOAT4& color) {
	// シェーダーを描画パイプラインに設定
	Shader_Begin();

	// 頂点バッファをロックする
	D3D11_MAPPED_SUBRESOURCE msr;
	g_pContext->Map(g_pVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

	// 頂点バッファへの仮想ポインタを取得
	Vertex* v = (Vertex*)msr.pData;

	//頂点情報を書き込み
	unsigned int dw = Texture_Width(texid);
	unsigned int dh = Texture_Height(texid);


	//画面左上から
	v[0].position = { dx		, dy		, 0.0f }; //z軸の値が同じ以下だと描画しない
	v[1].position = { dx + dw	, dy		, 0.0f };
	v[2].position = { dx		, dy + dh	, 0.0f };
	v[3].position = { dx + dw	, dy + dh	, 0.0f };

	//r,g,b,a
	v[0].color = color; //左上
	v[1].color = color; //右上
	v[2].color = color; //左下
	v[3].color = color; //右下

	v[0].texcoord = { 0.0f,0.0f };
	v[1].texcoord = { 1.0f,0.0f };
	v[2].texcoord = { 0.0f,1.0f };
	v[3].texcoord = { 1.0f,1.0f };



	// 頂点バッファのロックを解除
	g_pContext->Unmap(g_pVertexBuffer, 0);

	Shader_SetWorldMatrix(XMMatrixIdentity()); //単位行列

	// 頂点バッファを描画パイプラインに設定
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	g_pContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &stride, &offset);


	// プリミティブトポロジ設定
	g_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	//テクスチャセット
	Texture_SetTexture(texid);

	// ポリゴン描画命令発行
	g_pContext->Draw(NUM_VERTEX, 0);
}

//テクスチャ全表示(表示サイズ変更)
void Sprite_Draw(int texid, float dx, float dy, float dw, float dh, const XMFLOAT4& color) {
	// シェーダーを描画パイプラインに設定
	Shader_Begin();

	// 頂点バッファをロックする
	D3D11_MAPPED_SUBRESOURCE msr;
	g_pContext->Map(g_pVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

	// 頂点バッファへの仮想ポインタを取得
	Vertex* v = (Vertex*)msr.pData;


	//画面左上から
	v[0].position = { dx		, dy		, 0.0f }; //z軸の値が同じ以下だと描画しない
	v[1].position = { dx + dw	, dy		, 0.0f };
	v[2].position = { dx		, dy + dh	, 0.0f };
	v[3].position = { dx + dw	, dy + dh	, 0.0f };

	//r,g,b,a
	v[0].color = color; //左上
	v[1].color = color; //右上
	v[2].color = color; //左下
	v[3].color = color; //右下

	v[0].texcoord = { 0.0f,0.0f };
	v[1].texcoord = { 1.0f,0.0f };
	v[2].texcoord = { 0.0f,1.0f };
	v[3].texcoord = { 1.0f,1.0f };



	// 頂点バッファのロックを解除
	g_pContext->Unmap(g_pVertexBuffer, 0);

	Shader_SetWorldMatrix(XMMatrixIdentity()); //単位行列

	// 頂点バッファを描画パイプラインに設定
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	g_pContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &stride, &offset);


	// プリミティブトポロジ設定
	g_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	//テクスチャセット
	Texture_SetTexture(texid);

	// ポリゴン描画命令発行
	g_pContext->Draw(NUM_VERTEX, 0);
}

//UVカット
//ピクセル座標で指定
//px,pyは左上、pw,phは幅と高さ
void Sprite_Draw(int texid, float dx, float dy, int px, int py, int pw, int ph, const XMFLOAT4& color) {
	// シェーダーを描画パイプラインに設定
	Shader_Begin();

	// 頂点バッファをロックする
	D3D11_MAPPED_SUBRESOURCE msr;
	g_pContext->Map(g_pVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

	// 頂点バッファへの仮想ポインタを取得
	Vertex* v = (Vertex*)msr.pData;


	//画面左上から
	v[0].position = { dx		, dy		, 0.0f }; //z軸の値が同じ以下だと描画しない
	v[1].position = { dx + pw	, dy		, 0.0f };
	v[2].position = { dx		, dy + ph	, 0.0f };
	v[3].position = { dx + pw	, dy + ph	, 0.0f };

	//r,g,b,a
	v[0].color = color; //左上
	v[1].color = color; //右上
	v[2].color = color; //左下
	v[3].color = color; //右下


	float tw = (float)Texture_Width(texid);
	float th = (float)Texture_Height(texid);

	float u0 = px / tw;
	float v0 = py / th;
	float u1 = (px + pw) / tw;
	float v1 = (py + ph) / th;

	v[0].texcoord = { u0, v0 };
	v[1].texcoord = { u1, v0 };
	v[2].texcoord = { u0, v1 };
	v[3].texcoord = { u1, v1 };



	// 頂点バッファのロックを解除
	g_pContext->Unmap(g_pVertexBuffer, 0);

	Shader_SetWorldMatrix(XMMatrixIdentity()); //単位行列

	// 頂点バッファを描画パイプラインに設定
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	g_pContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &stride, &offset);


	// プリミティブトポロジ設定
	g_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	//テクスチャセット
	Texture_SetTexture(texid);

	// ポリゴン描画命令発行
	g_pContext->Draw(NUM_VERTEX, 0);
}

//UVカット(表示サイズ変更)
void Sprite_Draw(int texid, float dx, float dy, float dw, float dh, int px, int py, int pw, int ph, bool isHFlipped, bool isVFlipped, const XMFLOAT4& color) {
	// シェーダーを描画パイプラインに設定
	Shader_Begin();

	// 頂点バッファをロックする
	D3D11_MAPPED_SUBRESOURCE msr;
	g_pContext->Map(g_pVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

	// 頂点バッファへの仮想ポインタを取得
	Vertex* v = (Vertex*)msr.pData;


	//画面左上から
	v[0].position = { dx		, dy		, 0.0f }; //z軸の値が同じ以下だと描画しない
	v[1].position = { dx + dw	, dy		, 0.0f };
	v[2].position = { dx		, dy + dh	, 0.0f };
	v[3].position = { dx + dw	, dy + dh	, 0.0f };

	//r,g,b,a
	v[0].color = color; //左上
	v[1].color = color; //右上
	v[2].color = color; //左下
	v[3].color = color; //右下


	float tw = (float)Texture_Width(texid);
	float th = (float)Texture_Height(texid);

	float u0 = px / tw;
	float v0 = py / th;
	float u1 = (px + pw) / tw;
	float v1 = (py + ph) / th;

	//左右反転表示
	if (isHFlipped) {
		v[0].texcoord = { u1, v0 };
		v[1].texcoord = { u0, v0 };
		v[2].texcoord = { u1, v1 };
		v[3].texcoord = { u0, v1 };
	}
	//上下反転表示
	else if (isVFlipped) {
		v[0].texcoord = { u0, v1 };
		v[1].texcoord = { u1, v1 };
		v[2].texcoord = { u0, v0 };
		v[3].texcoord = { u1, v0 };
	}
	else {
		v[0].texcoord = { u0, v0 };
		v[1].texcoord = { u1, v0 };
		v[2].texcoord = { u0, v1 };
		v[3].texcoord = { u1, v1 };
	}


	// 頂点バッファのロックを解除
	g_pContext->Unmap(g_pVertexBuffer, 0);

	//ワールド変換行列
	Shader_SetWorldMatrix(XMMatrixIdentity()); //単位行列

	// 頂点バッファを描画パイプラインに設定
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	g_pContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &stride, &offset);


	// プリミティブトポロジ設定
	g_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	//テクスチャセット
	Texture_SetTexture(texid);

	// ポリゴン描画命令発行
	g_pContext->Draw(NUM_VERTEX, 0);
}


//UVカット(表示サイズ変更)回転
void Sprite_Draw(int texid, float dx, float dy, float dw, float dh, int px, int py, int pw, int ph, float angle, const XMFLOAT4& color) {
	// シェーダーを描画パイプラインに設定
	Shader_Begin();

	// 頂点バッファをロックする
	D3D11_MAPPED_SUBRESOURCE msr;
	g_pContext->Map(g_pVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

	// 頂点バッファへの仮想ポインタを取得
	Vertex* v = (Vertex*)msr.pData;


	//画面左上から右下に向かう線分を描画
	v[0].position = { -0.5f	, -0.5f	, 0.0f }; //z軸の値が同じ以下だと描画しない
	v[1].position = { +0.5f	, -0.5f	, 0.0f };
	v[2].position = { -0.5f	, +0.5f	, 0.0f };
	v[3].position = { +0.5f	, +0.5f	, 0.0f };

	//r,g,b,a
	v[0].color = color; //左上
	v[1].color = color; //右上
	v[2].color = color; //左下
	v[3].color = color; //右下


	float tw = (float)Texture_Width(texid);
	float th = (float)Texture_Height(texid);

	float u0 = px / tw;
	float v0 = py / th;
	float u1 = (px + pw) / tw;
	float v1 = (py + ph) / th;

	v[0].texcoord = { u0, v0 };
	v[1].texcoord = { u1, v0 };
	v[2].texcoord = { u0, v1 };
	v[3].texcoord = { u1, v1 };


	// 頂点バッファのロックを解除
	g_pContext->Unmap(g_pVertexBuffer, 0);


	//回転行列をシェーダに設定
	XMMATRIX scale = XMMatrixScaling(dw, dh, 1.0f);//拡大
	XMMATRIX rotation = XMMatrixRotationZ(angle);//回転
	XMMATRIX translation = XMMatrixTranslation(dx + dw * 0.5f, dy + dh * 0.5f, 0.0f);//平行移動
	Shader_SetWorldMatrix(scale * rotation * translation); //合成

	// 頂点バッファを描画パイプラインに設定
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	g_pContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &stride, &offset);


	// プリミティブトポロジ設定
	g_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	//テクスチャセット
	Texture_SetTexture(texid);

	// ポリゴン描画命令発行
	g_pContext->Draw(NUM_VERTEX, 0);
}
