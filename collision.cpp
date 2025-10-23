/*==============================================================================

   �R���W�������� [collision.cpp]
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


static constexpr int NUM_VERTEX = 5000; // ���_��


static ID3D11Buffer* g_pVertexBuffer = nullptr; // ���_�o�b�t�@

static ID3D11Device* g_pDevice = nullptr;
static ID3D11DeviceContext* g_pContext = nullptr;

static int g_WhiteTex = -1;


// ���_�\����
struct Vertex
{
	XMFLOAT3 position; // ���_���W
	XMFLOAT4 color; //���_�J���[
	XMFLOAT2 texcoord;//�e�N�X�`�����W(UV)
};


//�~�̓����蔻��
bool Collision_IsOverlapCircle(const Circle& a, const Circle& b){
	//a����Ƃ��ċ����𑪂�
	float x1 = b.center.x - a.center.x;
	float y1 = b.center.y - a.center.y;
	
	return (a.radius + b.radius) * (a.radius + b.radius) > (x1 * x1 + y1 * y1);
}

//�l�p�̓����蔻��
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
	// �~�̒��S�ɍł��߂��l�p�`��̓_�����߂�
	float closest_x = std::max(box.center.x - box.half_width, std::min(circle.center.x, box.center.x + box.half_width));
	float closest_y = std::max(box.center.y - box.half_height, std::min(circle.center.y, box.center.y + box.half_height));

	// �ł��߂��_�Ɖ~�̒��S�Ƃ̋������v�Z
	float distance_x = circle.center.x - closest_x;
	float distance_y = circle.center.y - closest_y;
	float distance_sq = (distance_x * distance_x) + (distance_y * distance_y); // ������2��

	// ������2�悪�~�̔��a��2���菬������ΏՓ˂��Ă���
	return distance_sq < (circle.radius * circle.radius);
}

bool Collision_IsOverlapOBBVSCircle(const OBB& obb, const Circle& circle)
{
	// �~�̒��S��OBB�̃��[�J�����W�n�ɕϊ�����
	XMVECTOR obb_center = XMLoadFloat2(&obb.center);
	XMVECTOR circle_center = XMLoadFloat2(&circle.center);
	XMVECTOR vec_to_circle = circle_center - obb_center; // OBB�̒��S����~�̒��S�ւ̃x�N�g��

	XMVECTOR obb_axis0 = XMLoadFloat2(&obb.axis[0]);
	XMVECTOR obb_axis1 = XMLoadFloat2(&obb.axis[1]);

	// �h�b�g�ς��g���āAOBB�̃��[�J�����W�n�ł̉~�̒��S���W���v�Z
	XMFLOAT2 circle_center_local;
	circle_center_local.x = XMVectorGetX(XMVector2Dot(vec_to_circle, obb_axis0));
	circle_center_local.y = XMVectorGetY(XMVector2Dot(vec_to_circle, obb_axis1));


	// OBB�̕ӏ�ŉ~�̒��S�ɍł��߂��_�����߂�
	float closest_x = std::max(-obb.half_extent.x, std::min(circle_center_local.x, obb.half_extent.x));
	float closest_y = std::max(-obb.half_extent.y, std::min(circle_center_local.y, obb.half_extent.y));


	//�ł��߂��_�Ɖ~�̒��S�Ƃ̋������v�Z
	float distance_x = circle_center_local.x - closest_x;
	float distance_y = circle_center_local.y - closest_y;
	float distance_sq = (distance_x * distance_x) + (distance_y * distance_y); // ������2��


	// ������2�悪�~�̔��a��2���菬������ΏՓ˂��Ă���
	return distance_sq < (circle.radius * circle.radius);
}

bool Collision_IsOverlapOBBVSBox(const OBB& obb, const Box& box){
	//Box��OBB�Ƃ݂Ȃ���OBB�ǂ����̓����蔻�������
	//Box��OBB�ɕϊ�����
	OBB box_as_obb;
	box_as_obb.center = box.center;
	box_as_obb.half_extent = { box.half_width, box.half_height };
	//��]���Ă��Ȃ��̂Ŏ��x�N�g���͕W����(1,0)��(0,1)
	box_as_obb.axis[0] = { 1.0f, 0.0f };
	box_as_obb.axis[1] = { 0.0f, 1.0f };

	//OBB vs OBB �̓����蔻��֐����Ăяo���āA���̌��ʂ�Ԃ�
	return Collision_IsOverlapOBB(obb, box_as_obb);
}

bool Collision_IsOverlapOBB(const OBB& a, const OBB& b){
	//�`�F�b�N����K�v������4�̎��i���݂���X����Y���j
	DirectX::XMFLOAT2 axes[4] = {
		a.axis[0], a.axis[1],
		b.axis[0], b.axis[1]
	};

	for (int i = 0; i < 4; ++i) {
		float a_min, a_max, b_min, b_max;
		//�e���ɑ΂��Ă��ꂼ���OBB�̉e�͈̔͂��v�Z
		ProjectOBB(&a_min, &a_max, a, axes[i]);
		ProjectOBB(&b_min, &b_max, b, axes[i]);

		//�e�����S�ɗ���Ă���i�d�Ȃ��Ă��Ȃ��j������ł�������ΏՓ˂��Ă��Ȃ�
		if (a_max < b_min || b_max < a_min) {
			return false;
		}
	}

	// 4�̎����ׂĂŉe���d�Ȃ��Ă����ꍇ�Փ˂��Ă���
	return true;
}

static void ProjectOBB(float* min, float* max, const OBB& obb, const DirectX::XMFLOAT2& axis){
	//OBB�̒��S�_�����Ɏˉe����
	float p = obb.center.x * axis.x + obb.center.y * axis.y;

	//���S�_����e�̒[�܂ł̍ő勗�������߂�
	//OBB��2�̎��x�N�g�����A���莲�ɂ��ꂼ��ˉe���A�����̒������|����
	float r = obb.half_extent.x * (float)fabs(obb.axis[0].x * axis.x + obb.axis[0].y * axis.y) +
		obb.half_extent.y * (float)fabs(obb.axis[1].x * axis.x + obb.axis[1].y * axis.y);

	//���S�̎ˉe����A�e�̒[�܂ł̋����𑫂��������āA�e�̍ŏ��l�ƍő�l�����߂�
	*min = p - r;
	*max = p + r;
}

void Collision_DebugInitialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext){
	//�f�o�C�X�ƃf�o�C�X�R���e�L�X�g�̕ۑ�
	g_pDevice = pDevice;
	g_pContext = pContext;
	
	//���_�o�b�t�@����
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
	//�_�̐����Z�o
	int NumVertex = (int)(circle.radius * 2.0f + XM_PI + 1);

	//�V�F�[�_�[��`��p�C�v���C���ɐݒ�
	Shader_Begin();

	//�J�����̃I�t�Z�b�g���擾
	XMFLOAT2 camera_offset = Map_GetWorldOffset();

	//���[���h���W�ł���box�̒��S����A�J�����̍��W�������ăX�N���[�����W�ɕϊ�
	float screen_x = circle.center.x - camera_offset.x;
	float screen_y = circle.center.y - camera_offset.y;

	//���_�o�b�t�@�����b�N����
	D3D11_MAPPED_SUBRESOURCE msr;
	g_pContext->Map(g_pVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

	//���_�o�b�t�@�ւ̉��z�|�C���^���擾
	Vertex* v = (Vertex*)msr.pData;

	const float rad = XM_2PI / NumVertex;

	for (int i = 0;i < NumVertex; i++) {
		v[i].position.x = cosf(rad * i) * circle.radius + screen_x;
		v[i].position.y = sinf(rad * i) * circle.radius + screen_y;
		v[i].position.z = 0.0f;
		v[i].color = color;
		v[i].texcoord = { 0.0f,0.0f };
	}


	//���_�o�b�t�@�̃��b�N������
	g_pContext->Unmap(g_pVertexBuffer, 0);

	//���[���h�}�g���N�X�ɃZ�b�g
	Shader_SetWorldMatrix(XMMatrixIdentity()); //�P�ʍs��

	//���_�o�b�t�@��`��p�C�v���C���ɐݒ�
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	g_pContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &stride, &offset);

	//�v���~�e�B�u�g�|���W�ݒ�
	g_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	//�e�N�X�`���Z�b�g
	Texture_SetTexture(g_WhiteTex);

	//�|���S���`�施�ߔ��s
	g_pContext->Draw(NumVertex, 0);

}

void Collision_DebugDraw(const Box& box, const DirectX::XMFLOAT4& color){
	//�V�F�[�_�[��`��p�C�v���C���ɐݒ�
	Shader_Begin();

	//�J�����̃I�t�Z�b�g���擾
	XMFLOAT2 camera_offset = Map_GetWorldOffset();

	//���[���h���W�ł���box�̒��S����A�J�����̍��W�������ăX�N���[�����W�ɕϊ�
	float screen_x = box.center.x - camera_offset.x;
	float screen_y = box.center.y - camera_offset.y;

	//���_�o�b�t�@�����b�N����
	D3D11_MAPPED_SUBRESOURCE msr;
	g_pContext->Map(g_pVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

	//���_�o�b�t�@�ւ̉��z�|�C���^���擾
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


	//���_�o�b�t�@�̃��b�N������
	g_pContext->Unmap(g_pVertexBuffer, 0);

	//���[���h�}�g���N�X�ɃZ�b�g
	Shader_SetWorldMatrix(XMMatrixIdentity()); //�P�ʍs��

	//���_�o�b�t�@��`��p�C�v���C���ɐݒ�
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	g_pContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &stride, &offset);

	//�v���~�e�B�u�g�|���W�ݒ�
	g_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);

	//�e�N�X�`���Z�b�g
	Texture_SetTexture(g_WhiteTex);

	//�|���S���`�施�ߔ��s
	g_pContext->Draw(5, 0);
}

void Collision_DebugDraw(const OBB& obb, const DirectX::XMFLOAT4& color) {
	//OBB�̔����̃T�C�Y�Ǝ��x�N�g����XMVECTOR�Ƀ��[�h
	XMVECTOR half_extent_x = XMLoadFloat2(&obb.axis[0]) * obb.half_extent.x;
	XMVECTOR half_extent_y = XMLoadFloat2(&obb.axis[1]) * obb.half_extent.y;
	XMVECTOR center = XMLoadFloat2(&obb.center);

	//OBB��4�̒��_�̃��[���h���W���v�Z
	XMFLOAT2 corners[4];
	XMStoreFloat2(&corners[0], center - half_extent_x - half_extent_y); // ����
	XMStoreFloat2(&corners[1], center + half_extent_x - half_extent_y); // �E��
	XMStoreFloat2(&corners[2], center + half_extent_x + half_extent_y); // �E��
	XMStoreFloat2(&corners[3], center - half_extent_x + half_extent_y); // ����

	//�J�����̃I�t�Z�b�g���擾���āA�e���_���X�N���[�����W�ɕϊ�
	XMFLOAT2 Offset = Map_GetWorldOffset();
	for (int i = 0; i < 4; ++i) {
		corners[i].x -= Offset.x;
		corners[i].y -= Offset.y;
	}

	//�`��̂��߂̒��_�o�b�t�@������
	static constexpr int NumVertex = 5;
	D3D11_MAPPED_SUBRESOURCE msr;
	g_pContext->Map(g_pVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
	Vertex* v = (Vertex*)msr.pData;

	//4�̒��_����Ō��Ԃ悤�ɐݒ� (�Ō�͎n�_�ɖ߂�)
	v[0].position = { corners[0].x, corners[0].y, 0.0f };
	v[1].position = { corners[1].x, corners[1].y, 0.0f };
	v[2].position = { corners[2].x, corners[2].y, 0.0f };
	v[3].position = { corners[3].x, corners[3].y, 0.0f };
	v[4].position = { corners[0].x, corners[0].y, 0.0f }; // �n�_�ɖ߂�

	for (int i = 0; i < NumVertex; ++i) {
		v[i].color = color;
		v[i].texcoord = { 0.0f, 0.0f };
	}
	g_pContext->Unmap(g_pVertexBuffer, 0);

	//�`��p�C�v���C���̐ݒ�ƕ`�施��
	Shader_Begin();
	Shader_SetWorldMatrix(XMMatrixIdentity());
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	g_pContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &stride, &offset);
	g_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP); // ����`�悷�郂�[�h
	Texture_SetTexture(-1); // �e�N�X�`���͎g��Ȃ�
	g_pContext->Draw(NumVertex, 0);
}