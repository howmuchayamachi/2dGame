/*====================================================================================

�e�N�X�`���Ǘ�[texture.cpp]

																Author	: Harada Ren
																Date	: 2025/06/13
--------------------------------------------------------------------------------------

======================================================================================*/

#include "texture.h"
#include "direct3d.h"
#include "DirectXTex.h"
using namespace DirectX;
#include <string>


//�e�N�X�`���Ǘ��ő吔
static constexpr int TEXTURE_MAX = 512;

struct Texture {
	std::wstring filename;
	ID3D11ShaderResourceView* pTexture; //�e�N�X�`�� //static������
	unsigned int width;
	unsigned int height;
};

static Texture g_Textures[TEXTURE_MAX]{};
static int g_SetTextureIndex = -1;

// ���ӁI�������ŊO������ݒ肳�����́BRelease�s�v�B
static ID3D11Device* g_pDevice = nullptr;
static ID3D11DeviceContext* g_pContext = nullptr;

void Texture_Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext){
	for (Texture& t : g_Textures) {
		t.pTexture = nullptr; //������
	}

	g_SetTextureIndex = -1;

	// �f�o�C�X�ƃf�o�C�X�R���e�L�X�g�̕ۑ�
	g_pDevice = pDevice;
	g_pContext = pContext;

}

void Texture_Finalize(void){
	Texture_AllRelease();
}

int Texture_Load(const wchar_t* pFilename){
	//���ɓǂݍ���ł���t�@�C���͓ǂݍ��܂Ȃ�
	for (int i = 0; i < TEXTURE_MAX; i++) {
		if (g_Textures[i].filename == pFilename) {
			return i;
		}
	}

	//�󂢂Ă���Ǘ��̈��T��
	for (int i = 0; i < TEXTURE_MAX; i++) {
		//�g�p��(nullptr����Ȃ�������)�������玟�̗̈��
		if (g_Textures[i].pTexture) {
			continue;
		}
		//�e�N�X�`���̓ǂݍ���
		TexMetadata metadata;
		ScratchImage image;

		//�t�@�C���̓ǂݍ��݂����ă��^�f�[�^�����
		HRESULT hr = LoadFromWICFile(pFilename, WIC_FLAGS_NONE, &metadata, image);
		if (FAILED(hr)) {
			MessageBoxW(nullptr, L"�e�N�X�`���̓ǂݍ��݂Ɏ��s���܂���", pFilename, MB_OK);
			return -1;
		}

		g_Textures[i].filename = pFilename;
		g_Textures[i].width = (unsigned int)metadata.width;
		g_Textures[i].height = (unsigned int)metadata.height;


		hr = CreateShaderResourceView(g_pDevice, image.GetImages(), image.GetImageCount(), metadata, &g_Textures[i].pTexture);
		return i; //�Ǘ��ԍ�
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
		return; //-1�������牽�����Ȃ��ŕԂ�
	}

	g_SetTextureIndex = texid;
	//�e�N�X�`���ݒ�
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
