/*====================================================================================

�}�E�X���͂̋L�^[mouse.h]

																Author	: Harada Ren
																Date	: 2025/06/27
--------------------------------------------------------------------------------------

======================================================================================*/
#ifndef MOUSE_H
#define MOUSE_H
#pragma once


#include <windows.h>
#include <memory>


// �}�E�X���[�h
typedef enum Mouse_PositionMode_tag
{
	MOUSE_POSITION_MODE_ABSOLUTE, // ��΍��W���[�h
	MOUSE_POSITION_MODE_RELATIVE, // ���΍��W���[�h
} Mouse_PositionMode;


// �}�E�X��ԍ\����
typedef struct MouseState_tag
{
	bool leftButton;
	bool middleButton;
	bool rightButton;
	bool xButton1;
	bool xButton2;
	int x;
	int y;
	int scrollWheelValue;
	Mouse_PositionMode positionMode;
} Mouse_State;


// �}�E�X���W���[���̏�����
void Mouse_Initialize(HWND window);

// �}�E�X���W���[���̏I������
void Mouse_Finalize(void);

// �}�E�X�̏�Ԃ��擾����
void Mouse_GetState(Mouse_State* pState);

// �ݐς����}�E�X�X�N���[���z�C�[���l�����Z�b�g����
void Mouse_ResetScrollWheelValue(void);

// �}�E�X�̃|�W�V�������[�h��ݒ肷��i�f�t�H���g�͐�΍��W���[�h�j
void Mouse_SetMode(Mouse_PositionMode mode);

// �}�E�X�̐ڑ������o����
bool Mouse_IsConnected(void);

// �}�E�X�J�[�\�����\������Ă��邩�m�F����
bool Mouse_IsVisible(void);

// �}�E�X�J�[�\���\����ݒ肷��
void Mouse_SetVisible(bool visible);

// �}�E�X����̂��߂̃E�B���h�E���b�Z�[�W�v���V�[�W���t�b�N�֐�
void Mouse_ProcessMessage(UINT message, WPARAM wParam, LPARAM lParam);


#endif // MOUSE_H
