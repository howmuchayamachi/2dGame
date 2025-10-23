/*==============================================================================

   �X�v���C�g�`�� [sprite.cpp]
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



static constexpr int NUM_VERTEX = 4; // ���_��


static ID3D11Buffer* g_pVertexBuffer = nullptr; // ���_�o�b�t�@
static ID3D11ShaderResourceView* g_pTexture = nullptr; //�e�N�X�`��

static ID3D11Device* g_pDevice = nullptr;
static ID3D11DeviceContext* g_pContext = nullptr;


// ���_�\����
struct Vertex {
	XMFLOAT3 position; // ���_���W

	XMFLOAT4 color; //���_�J���[

	XMFLOAT2 texcoord;//�e�N�X�`�����W(UV)
};


void Sprite_Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) {
	// �f�o�C�X�ƃf�o�C�X�R���e�L�X�g�̃`�F�b�N
	if (!pDevice || !pContext) {
		hal::dout << "Sprite_Initialize() : �^����ꂽ�f�o�C�X���R���e�L�X�g���s���ł�" << std::endl;
		return;
	}

	// �f�o�C�X�ƃf�o�C�X�R���e�L�X�g�̕ۑ�
	g_pDevice = pDevice;
	g_pContext = pContext;

	// ���_�o�b�t�@����
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
	// ���_������������
	const float SCREEN_WIDTH = (float)Direct3D_GetBackBufferWidth();
	const float SCREEN_HEIGHT = (float)Direct3D_GetBackBufferHeight();
	// ���_�V�F�[�_�[�ɕϊ��s���ݒ�
	Shader_SetProjectionMatrix(XMMatrixOrthographicOffCenterLH(0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f, 0.0f, 1.0f));

}

//�e�N�X�`���S�\��
void Sprite_Draw(int texid, float dx, float dy, const XMFLOAT4& color) {
	// �V�F�[�_�[��`��p�C�v���C���ɐݒ�
	Shader_Begin();

	// ���_�o�b�t�@�����b�N����
	D3D11_MAPPED_SUBRESOURCE msr;
	g_pContext->Map(g_pVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

	// ���_�o�b�t�@�ւ̉��z�|�C���^���擾
	Vertex* v = (Vertex*)msr.pData;

	//���_������������
	unsigned int dw = Texture_Width(texid);
	unsigned int dh = Texture_Height(texid);


	//��ʍ��ォ��
	v[0].position = { dx		, dy		, 0.0f }; //z���̒l�������ȉ����ƕ`�悵�Ȃ�
	v[1].position = { dx + dw	, dy		, 0.0f };
	v[2].position = { dx		, dy + dh	, 0.0f };
	v[3].position = { dx + dw	, dy + dh	, 0.0f };

	//r,g,b,a
	v[0].color = color; //����
	v[1].color = color; //�E��
	v[2].color = color; //����
	v[3].color = color; //�E��

	v[0].texcoord = { 0.0f,0.0f };
	v[1].texcoord = { 1.0f,0.0f };
	v[2].texcoord = { 0.0f,1.0f };
	v[3].texcoord = { 1.0f,1.0f };



	// ���_�o�b�t�@�̃��b�N������
	g_pContext->Unmap(g_pVertexBuffer, 0);

	Shader_SetWorldMatrix(XMMatrixIdentity()); //�P�ʍs��

	// ���_�o�b�t�@��`��p�C�v���C���ɐݒ�
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	g_pContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &stride, &offset);


	// �v���~�e�B�u�g�|���W�ݒ�
	g_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	//�e�N�X�`���Z�b�g
	Texture_SetTexture(texid);

	// �|���S���`�施�ߔ��s
	g_pContext->Draw(NUM_VERTEX, 0);
}

//�e�N�X�`���S�\��(�\���T�C�Y�ύX)
void Sprite_Draw(int texid, float dx, float dy, float dw, float dh, const XMFLOAT4& color) {
	// �V�F�[�_�[��`��p�C�v���C���ɐݒ�
	Shader_Begin();

	// ���_�o�b�t�@�����b�N����
	D3D11_MAPPED_SUBRESOURCE msr;
	g_pContext->Map(g_pVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

	// ���_�o�b�t�@�ւ̉��z�|�C���^���擾
	Vertex* v = (Vertex*)msr.pData;


	//��ʍ��ォ��
	v[0].position = { dx		, dy		, 0.0f }; //z���̒l�������ȉ����ƕ`�悵�Ȃ�
	v[1].position = { dx + dw	, dy		, 0.0f };
	v[2].position = { dx		, dy + dh	, 0.0f };
	v[3].position = { dx + dw	, dy + dh	, 0.0f };

	//r,g,b,a
	v[0].color = color; //����
	v[1].color = color; //�E��
	v[2].color = color; //����
	v[3].color = color; //�E��

	v[0].texcoord = { 0.0f,0.0f };
	v[1].texcoord = { 1.0f,0.0f };
	v[2].texcoord = { 0.0f,1.0f };
	v[3].texcoord = { 1.0f,1.0f };



	// ���_�o�b�t�@�̃��b�N������
	g_pContext->Unmap(g_pVertexBuffer, 0);

	Shader_SetWorldMatrix(XMMatrixIdentity()); //�P�ʍs��

	// ���_�o�b�t�@��`��p�C�v���C���ɐݒ�
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	g_pContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &stride, &offset);


	// �v���~�e�B�u�g�|���W�ݒ�
	g_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	//�e�N�X�`���Z�b�g
	Texture_SetTexture(texid);

	// �|���S���`�施�ߔ��s
	g_pContext->Draw(NUM_VERTEX, 0);
}

//UV�J�b�g
//�s�N�Z�����W�Ŏw��
//px,py�͍���Apw,ph�͕��ƍ���
void Sprite_Draw(int texid, float dx, float dy, int px, int py, int pw, int ph, const XMFLOAT4& color) {
	// �V�F�[�_�[��`��p�C�v���C���ɐݒ�
	Shader_Begin();

	// ���_�o�b�t�@�����b�N����
	D3D11_MAPPED_SUBRESOURCE msr;
	g_pContext->Map(g_pVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

	// ���_�o�b�t�@�ւ̉��z�|�C���^���擾
	Vertex* v = (Vertex*)msr.pData;


	//��ʍ��ォ��
	v[0].position = { dx		, dy		, 0.0f }; //z���̒l�������ȉ����ƕ`�悵�Ȃ�
	v[1].position = { dx + pw	, dy		, 0.0f };
	v[2].position = { dx		, dy + ph	, 0.0f };
	v[3].position = { dx + pw	, dy + ph	, 0.0f };

	//r,g,b,a
	v[0].color = color; //����
	v[1].color = color; //�E��
	v[2].color = color; //����
	v[3].color = color; //�E��


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



	// ���_�o�b�t�@�̃��b�N������
	g_pContext->Unmap(g_pVertexBuffer, 0);

	Shader_SetWorldMatrix(XMMatrixIdentity()); //�P�ʍs��

	// ���_�o�b�t�@��`��p�C�v���C���ɐݒ�
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	g_pContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &stride, &offset);


	// �v���~�e�B�u�g�|���W�ݒ�
	g_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	//�e�N�X�`���Z�b�g
	Texture_SetTexture(texid);

	// �|���S���`�施�ߔ��s
	g_pContext->Draw(NUM_VERTEX, 0);
}

//UV�J�b�g(�\���T�C�Y�ύX)
void Sprite_Draw(int texid, float dx, float dy, float dw, float dh, int px, int py, int pw, int ph, bool isHFlipped, bool isVFlipped, const XMFLOAT4& color) {
	// �V�F�[�_�[��`��p�C�v���C���ɐݒ�
	Shader_Begin();

	// ���_�o�b�t�@�����b�N����
	D3D11_MAPPED_SUBRESOURCE msr;
	g_pContext->Map(g_pVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

	// ���_�o�b�t�@�ւ̉��z�|�C���^���擾
	Vertex* v = (Vertex*)msr.pData;


	//��ʍ��ォ��
	v[0].position = { dx		, dy		, 0.0f }; //z���̒l�������ȉ����ƕ`�悵�Ȃ�
	v[1].position = { dx + dw	, dy		, 0.0f };
	v[2].position = { dx		, dy + dh	, 0.0f };
	v[3].position = { dx + dw	, dy + dh	, 0.0f };

	//r,g,b,a
	v[0].color = color; //����
	v[1].color = color; //�E��
	v[2].color = color; //����
	v[3].color = color; //�E��


	float tw = (float)Texture_Width(texid);
	float th = (float)Texture_Height(texid);

	float u0 = px / tw;
	float v0 = py / th;
	float u1 = (px + pw) / tw;
	float v1 = (py + ph) / th;

	//���E���]�\��
	if (isHFlipped) {
		v[0].texcoord = { u1, v0 };
		v[1].texcoord = { u0, v0 };
		v[2].texcoord = { u1, v1 };
		v[3].texcoord = { u0, v1 };
	}
	//�㉺���]�\��
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


	// ���_�o�b�t�@�̃��b�N������
	g_pContext->Unmap(g_pVertexBuffer, 0);

	//���[���h�ϊ��s��
	Shader_SetWorldMatrix(XMMatrixIdentity()); //�P�ʍs��

	// ���_�o�b�t�@��`��p�C�v���C���ɐݒ�
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	g_pContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &stride, &offset);


	// �v���~�e�B�u�g�|���W�ݒ�
	g_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	//�e�N�X�`���Z�b�g
	Texture_SetTexture(texid);

	// �|���S���`�施�ߔ��s
	g_pContext->Draw(NUM_VERTEX, 0);
}


//UV�J�b�g(�\���T�C�Y�ύX)��]
void Sprite_Draw(int texid, float dx, float dy, float dw, float dh, int px, int py, int pw, int ph, float angle, const XMFLOAT4& color) {
	// �V�F�[�_�[��`��p�C�v���C���ɐݒ�
	Shader_Begin();

	// ���_�o�b�t�@�����b�N����
	D3D11_MAPPED_SUBRESOURCE msr;
	g_pContext->Map(g_pVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

	// ���_�o�b�t�@�ւ̉��z�|�C���^���擾
	Vertex* v = (Vertex*)msr.pData;


	//��ʍ��ォ��E���Ɍ�����������`��
	v[0].position = { -0.5f	, -0.5f	, 0.0f }; //z���̒l�������ȉ����ƕ`�悵�Ȃ�
	v[1].position = { +0.5f	, -0.5f	, 0.0f };
	v[2].position = { -0.5f	, +0.5f	, 0.0f };
	v[3].position = { +0.5f	, +0.5f	, 0.0f };

	//r,g,b,a
	v[0].color = color; //����
	v[1].color = color; //�E��
	v[2].color = color; //����
	v[3].color = color; //�E��


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


	// ���_�o�b�t�@�̃��b�N������
	g_pContext->Unmap(g_pVertexBuffer, 0);


	//��]�s����V�F�[�_�ɐݒ�
	XMMATRIX scale = XMMatrixScaling(dw, dh, 1.0f);//�g��
	XMMATRIX rotation = XMMatrixRotationZ(angle);//��]
	XMMATRIX translation = XMMatrixTranslation(dx + dw * 0.5f, dy + dh * 0.5f, 0.0f);//���s�ړ�
	Shader_SetWorldMatrix(scale * rotation * translation); //����

	// ���_�o�b�t�@��`��p�C�v���C���ɐݒ�
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	g_pContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &stride, &offset);


	// �v���~�e�B�u�g�|���W�ݒ�
	g_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	//�e�N�X�`���Z�b�g
	Texture_SetTexture(texid);

	// �|���S���`�施�ߔ��s
	g_pContext->Draw(NUM_VERTEX, 0);
}
