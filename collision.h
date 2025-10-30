/*==============================================================================

   �R���W�������� [collision.h]
														 Author : Harada Ren
														 Date   : 2025/07/03
--------------------------------------------------------------------------------

==============================================================================*/
#ifndef COLLISION_H
#define COLLISION_H

#include <d3d11.h>
#include <DirectXMath.h>


struct Circle {
	DirectX::XMFLOAT2 center; //���S���W
	float radius; //���a
};

struct Box {
	DirectX::XMFLOAT2 center; //���S���W
	float half_width; //�����̕�
	float half_height; //�����̍���
};

//��]����Box
struct OBB {
	DirectX::XMFLOAT2 center; //���S���W
	DirectX::XMFLOAT2 half_extent; //�e�������̔����̒���
	DirectX::XMFLOAT2 axis[2]; //��]��\��2�̎��x�N�g��
};

//�~�ǂ����̓����蔻��
bool Collision_IsOverlapCircle(const Circle& a, const Circle& b);
//�l�p�ǂ����̓����蔻��
bool Collision_IsOverlapBox(const Box& a, const Box& b);
//�~�Ǝl�p�̂����蔻��
bool Collision_IsOverlapCircleVSBox(const Box& box, const Circle& circle);
// OBB�Ɖ~�̓����蔻��
bool Collision_IsOverlapOBBVSCircle(const OBB& obb, const Circle& circle);
//OBB�Ǝl�p�̓����蔻��
bool Collision_IsOverlapOBBVSBox(const OBB& obb, const Box& box);
//OBB�ǂ����̓����蔻��
bool Collision_IsOverlapOBB(const OBB& a, const OBB& b);

//�������̒藝�Ŏg�p����
static void ProjectOBB(float* min, float* max, const OBB& obb, const DirectX::XMFLOAT2& axis);

//�f�o�b�O�\��
void Collision_DebugInitialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
void Collision_DebugFinalize();
void Collision_DebugDraw(const Circle& circle, const DirectX::XMFLOAT4& color = { 1.0f,1.0f,1.0f,1.0f });
void Collision_DebugDraw(const Box& box, const DirectX::XMFLOAT4& color = { 1.0f,1.0f,1.0f,1.0f });
void Collision_DebugDraw(const OBB& obb, const DirectX::XMFLOAT4& color = { 1.0f,1.0f,1.0f,1.0f });



#endif //COLLISION_H