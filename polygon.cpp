/*==============================================================================

   �|���S���`�� [polygon.cpp]
														 Author : Harada Ren
														 Date   : 2025/05/15
--------------------------------------------------------------------------------

==============================================================================*/
#include <d3d11.h>
#include <DirectXMath.h>
#include "DirectXTex.h"
using namespace DirectX;
#include "direct3d.h"
#include "shader.h"
#include "debug_ostream.h"



static constexpr int NUM_VERTEX = 4; // ���_��


static ID3D11Buffer* g_pVertexBuffer = nullptr; // ���_�o�b�t�@
static ID3D11ShaderResourceView* g_pTexture = nullptr; //�e�N�X�`��

// ���ӁI�������ŊO������ݒ肳�����́BRelease�s�v�B
static ID3D11Device* g_pDevice = nullptr;
static ID3D11DeviceContext* g_pContext = nullptr;

static int g_NumVertex = 0;
static float g_Radius = 200.0f;
static float g_Cx = 500.0f;
static float g_Cy = 250.0f;


// ���_�\����
struct Vertex
{
	XMFLOAT3 position; // ���_���W

	//XMFLOAT4�cFLOAT�̒l��4�������
	XMFLOAT4 color; //���_�J���[

	XMFLOAT2 texcoord;//�e�N�X�`�����W(UV)
};


void Polygon_Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	// �f�o�C�X�ƃf�o�C�X�R���e�L�X�g�̃`�F�b�N
	if (!pDevice || !pContext) {
		hal::dout << "Polygon_Initialize() : �^����ꂽ�f�o�C�X���R���e�L�X�g���s���ł�" << std::endl;
		return;
	}

	// �f�o�C�X�ƃf�o�C�X�R���e�L�X�g�̕ۑ�
	g_pDevice = pDevice;
	g_pContext = pContext;

	//�_�̐����Z�o
	g_NumVertex = (int)(g_Radius * 2.0f + XM_PI);

	// ���_�o�b�t�@����
	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(Vertex) * g_NumVertex;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	g_pDevice->CreateBuffer(&bd, NULL, &g_pVertexBuffer);


	//�e�N�X�`���̓ǂݍ���
	TexMetadata metadata;
	ScratchImage image;

	//�t�@�C���̓ǂݍ��݂����ă��^�f�[�^�����
	LoadFromWICFile(L"resource/texture/white.png", WIC_FLAGS_NONE, &metadata, image); //�t�@�C�����擪��"L"�t����̂�Y�ꂸ��
	HRESULT hr = CreateShaderResourceView(g_pDevice, image.GetImages(), image.GetImageCount(), metadata, &g_pTexture);

	if (FAILED(hr)) {
		MessageBox(nullptr, "�e�N�X�`���̓ǂݍ��݂Ɏ��s���܂���", "�G���[", MB_OK);
	}
}

void Polygon_Finalize(void)
{
	SAFE_RELEASE(g_pTexture);
	SAFE_RELEASE(g_pVertexBuffer);
}

void Polygon_Draw(void)
{
	// �V�F�[�_�[��`��p�C�v���C���ɐݒ�
	Shader_Begin();

	// ���_�o�b�t�@�����b�N����
	D3D11_MAPPED_SUBRESOURCE msr;
	g_pContext->Map(g_pVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

	// ���_�o�b�t�@�ւ̉��z�|�C���^���擾
	Vertex* v = (Vertex*)msr.pData;


	// ���_������������
	const float SCREEN_WIDTH = (float)Direct3D_GetBackBufferWidth();
	const float SCREEN_HEIGHT = (float)Direct3D_GetBackBufferHeight();

	const float rad = XM_2PI / g_NumVertex;

	for (int i = 0;i < g_NumVertex; i++) {
		v[i].position.x = cosf(rad * i) * g_Radius + g_Cx;
		v[i].position.y = sinf(rad * i) * g_Radius + g_Cy;
		v[i].position.z = 0.0f;
		v[i].color = { 1.0f,1.0f,1.0f,1.0f };
		v[i].texcoord = { 0.0f,0.0f };
	}

	// ��ʂ̍��ォ��E���Ɍ�����������`�悷��
	//6���_�g����3�p�`2�Ŏl�p�`�����
	/*
	constexpr float x = 100.0f;
	constexpr float y = 100.0f;
	constexpr float w = 512.0f;
	constexpr float h = 512.0f;


	v[0].position = { x		, y		, 0.0f };
	v[1].position = { x+w	, y		, 0.0f };
	v[2].position = { x		, y+h	, 0.0f };
	v[3].position = { x+w	, y+h	, 0.0f };


	v[0].color = { 1.0f, 1.0f, 1.0f, 1.0f }; //r,g,b,a
	v[1].color = { 1.0f, 1.0f, 1.0f, 1.0f };
	v[2].color = { 1.0f, 1.0f, 1.0f, 1.0f };
	v[3].color = { 1.0f, 1.0f, 1.0f, 1.0f };


	v[0].texcoord = { 0.0f,0.0f };
	v[1].texcoord = { 1.0f,0.0f };
	v[2].texcoord = { 0.0f,1.0f };
	v[3].texcoord = { 1.0f,1.0f };
	*/




	// ���_�o�b�t�@�̃��b�N������
	g_pContext->Unmap(g_pVertexBuffer, 0);

	// ���_�o�b�t�@��`��p�C�v���C���ɐݒ�
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	g_pContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &stride, &offset);

	// ���_�V�F�[�_�[�ɕϊ��s���ݒ�
	Shader_SetProjectionMatrix(XMMatrixOrthographicOffCenterLH(0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f, 0.0f, 1.0f));

	// �v���~�e�B�u�g�|���W�ݒ�
	//POINTLIST�c�_��`��
	//LINELIST�c���_2�ɂ�1�{
	//LINESTRIP�c�������ׂĂȂ���
	//TRIANGLELIST�c���_3�ŎO�p�`�\��
	// �@�|���S���ɂ͗��\������
	// �@�E���ɒ��_��z�u���Ȃ��ƕ\�ɂȂ�Ȃ�(�\������Ȃ�)
	//TRIANGLESTRIP�c�т�`��(Z��orN���ɒ��_������)
	g_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	//�e�N�X�`���ݒ�
	//g_pContext->PSSetShaderResources(0, 1, &g_pTexture);

	// �|���S���`�施�ߔ��s
	g_pContext->Draw(g_NumVertex, 0);


}
