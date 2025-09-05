/*==============================================================================

  �^�C�g���f�o�b�O�p�e�L�X�g [debug_font.h]
														 Author : Harada Ren
														 Date   : 2025/08/05
--------------------------------------------------------------------------------

==============================================================================*/
#ifndef DEBUG_FONT_H
#define DEBUG_FONT_H

#include <DirectXMath.h>

void DebugFont_Initialize();
void DebugFont_Finalize();

// �������`�悷��֐��̐錾
void DebugFont_Draw(
	const char* text,      // �`�悵����������
	float x,               // �`��J�n�ʒu X���W
	float y,               // �`��J�n�ʒu Y���W
	const DirectX::XMFLOAT4& color = { 1.0f, 1.0f, 1.0f, 1.0f }, // �����̐F
	float scale = 1.0f     // �����̊g�嗦
);

#endif //DEBUG_FONT_H