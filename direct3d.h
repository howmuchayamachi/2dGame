/*==============================================================================

   Direct3D�̏������֘A [direct3d.cpp]
														 Author : Harada Ren
														 Date   : 2025/05/12
--------------------------------------------------------------------------------

==============================================================================*/
#ifndef DIRECT3D_H
#define DIRECT3D_H


#include <Windows.h>
#include <d3d11.h>


// �Z�[�t�����[�X�}�N��
#define SAFE_RELEASE(o) if (o) { (o)->Release(); o = NULL; }


bool Direct3D_Initialize(HWND hWnd); // Direct3D�̏�����
void Direct3D_Finalize(); // Direct3D�̏I������

void Direct3D_Clear(); // �o�b�N�o�b�t�@�̃N���A
void Direct3D_Present(); // �o�b�N�o�b�t�@�̕\��

//�o�b�N�o�b�t�@�̑傫���̎擾
unsigned int Direct3D_GetBackBufferWidth(); //��
unsigned int Direct3D_GetBackBufferHeight(); //����

//Direct3d�f�o�C�X�̎擾
ID3D11Device* Direct3D_GetDevice();
//Direct3d�f�o�C�X�R���e�L�X�g�̎擾
ID3D11DeviceContext* Direct3D_GetContext();

//���u�����h�ݒ�֐�
void Direct3D_SetAlphaBlendTransparent(); //���ߏ���
void Direct3D_SetAlphaBlendAdd();// ���Z����

#endif // DIRECT3D_H
