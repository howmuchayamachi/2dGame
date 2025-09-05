/*====================================================================================

テクスチャ管理[texture.cpp]

																Author	: Harada Ren
																Date	: 2025/06/13
--------------------------------------------------------------------------------------

======================================================================================*/

#include "texture.h"
#include "direct3d.h"
#include "DirectXTex.h"
using namespace DirectX;
#include <string>


//テクスチャ管理最大数
static constexpr int TEXTURE_MAX = 512;

struct Texture {
	std::wstring filename;
	ID3D11ShaderResourceView* pTexture; //テクスチャ //static消した
	unsigned int width;
	unsigned int height;
};

static Texture g_Textures[TEXTURE_MAX]{};
static int g_SetTextureIndex = -1;

// 注意！初期化で外部から設定されるもの。Release不要。
static ID3D11Device* g_pDevice = nullptr;
static ID3D11DeviceContext* g_pContext = nullptr;

void Texture_Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext){
	for (Texture& t : g_Textures) {
		t.pTexture = nullptr; //初期化
	}

	g_SetTextureIndex = -1;

	// デバイスとデバイスコンテキストの保存
	g_pDevice = pDevice;
	g_pContext = pContext;

}

void Texture_Finalize(void){
	Texture_AllRelease();
}

int Texture_Load(const wchar_t* pFilename){
	//既に読み込んでいるファイルは読み込まない
	for (int i = 0; i < TEXTURE_MAX; i++) {
		if (g_Textures[i].filename == pFilename) {
			return i;
		}
	}

	//空いている管理領域を探す
	for (int i = 0; i < TEXTURE_MAX; i++) {
		//使用中(nullptrじゃなかったら)だったら次の領域に
		if (g_Textures[i].pTexture) {
			continue;
		}
		//テクスチャの読み込み
		TexMetadata metadata;
		ScratchImage image;

		//ファイルの読み込みをしてメタデータを作る
		HRESULT hr = LoadFromWICFile(pFilename, WIC_FLAGS_NONE, &metadata, image);
		if (FAILED(hr)) {
			MessageBoxW(nullptr, L"テクスチャの読み込みに失敗しました", pFilename, MB_OK);
			return -1;
		}

		g_Textures[i].filename = pFilename;
		g_Textures[i].width = (unsigned int)metadata.width;
		g_Textures[i].height = (unsigned int)metadata.height;


		hr = CreateShaderResourceView(g_pDevice, image.GetImages(), image.GetImageCount(), metadata, &g_Textures[i].pTexture);
		return i; //管理番号
	}

	return -1;

}

void Texture_AllRelease(){
	for (Texture& t : g_Textures) {
		t.filename.clear();
		SAFE_RELEASE(t.pTexture);
	}
}

void Texture_SetTexture(int texid){
	if (texid < 0) {
		return; //-1だったら何もしないで返す
	}

	g_SetTextureIndex = texid;
	//テクスチャ設定
	g_pContext->PSSetShaderResources(0, 1, &g_Textures[texid].pTexture);
}

unsigned int Texture_Width(int texid){
	if (texid < 0) {
		return 0;
	}

	return g_Textures[texid].width;
}

unsigned int Texture_Height(int texid){
	if (texid < 0) {
		return 0;
	}

	return g_Textures[texid].height;
}
