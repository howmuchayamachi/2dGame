/*====================================================================================

�X�v���C�g�\��(�����|���S��)[sprite.h]

																Author	: Harada Ren
																Date	: 2025/06/06
--------------------------------------------------------------------------------------

======================================================================================*/
#ifndef SPRITE_H
#define SPRITE_H


#include <d3d11.h>
#include <DirectXMath.h>


void Sprite_Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
void Sprite_Finalize(void);

void Sprite_Begin();

//�e�N�X�`���ɂ���ĕ`����@��ς���
//�e�N�X�`���S�\��
void Sprite_Draw(int texid, float dx, float dy, const DirectX::XMFLOAT4& color = { 1.0f,1.0f,1.0f,1.0f }); //dxx���W,dyy���W,dw���Adh����

//�e�N�X�`���S�\��(�\���T�C�Y�ύX)
void Sprite_Draw(int texid, float dx, float dy, float dw, float dh, const DirectX::XMFLOAT4& color = { 1.0f,1.0f,1.0f,1.0f });

//UV�J�b�g
//�s�N�Z�����W�Ŏw��
//px,py�͍���Apw,ph�͕��ƍ���
void Sprite_Draw(int texid, float dx, float dy, int px, int py, int pw, int ph, const DirectX::XMFLOAT4& color = { 1.0f,1.0f,1.0f,1.0f });

//UV�J�b�g(�\���T�C�Y�ύX)
void Sprite_Draw(int texid, float dx, float dy, float dw, float dh, int px, int py, int pw, int ph, bool isVFlipped = false, bool isHflipped = false, const DirectX::XMFLOAT4& color = { 1.0f,1.0f,1.0f,1.0f });

//UV�J�b�g(�\���T�C�Y�ύX)�E��]
void Sprite_Draw(int texid, float dx, float dy, float dw, float dh, int px, int py, int pw, int ph, float angle, const DirectX::XMFLOAT4& color = { 1.0f,1.0f,1.0f,1.0f });

#endif //SPRITE_H
