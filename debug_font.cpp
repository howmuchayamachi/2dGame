/*==============================================================================

  �^�C�g���f�o�b�O�p�e�L�X�g [debug_font.cpp]
														 Author : Harada Ren
														 Date   : 2025/08/05
--------------------------------------------------------------------------------

==============================================================================*/
#include "debug_font.h"
#include "texture.h"
#include "sprite.h"
#include <string>

static const int FONT_CHAR_WIDTH = 32;   // �t�H���g�摜1����������̉���(�s�N�Z��)
static const int FONT_CHAR_HEIGHT = 32;  // �t�H���g�摜1����������̏c��(�s�N�Z��)
static const int FONT_CHARS_PER_ROW = 16; // �t�H���g�摜�ɉ�����������ł��邩

static int g_FontTextureId = -1; // �t�H���g�摜�̃e�N�X�`��ID

void DebugFont_Initialize(){
	g_FontTextureId = Texture_Load(L"resource/texture/consolab_ascii_512.png"); 
}

void DebugFont_Finalize(){
}

void DebugFont_Draw(const char* text, float x, float y, const DirectX::XMFLOAT4& color, float scale){
	if (g_FontTextureId < 0) return; // �e�N�X�`�����ǂݍ��܂�Ă��Ȃ���Ή������Ȃ�

	float current_x = x;
	int len = (int)strlen(text);

	// ��������ꕶ�������[�v���ĕ`��
	for (int i = 0; i < len; ++i) {
		unsigned char c = text[i];

		// ���s�����̏ꍇ�́AY���W�����炵��X���W�����Z�b�g
		if (c == '\n') {
			y += FONT_CHAR_HEIGHT * scale;
			current_x = x;
			continue;
		}

		// ASCII�R�[�h�����ɁA�t�H���g�摜�̂ǂ���؂蔲�����v�Z
		int char_index = (int)c;
		int cut_x = (char_index % FONT_CHARS_PER_ROW) * FONT_CHAR_WIDTH;
		int cut_y = (char_index / FONT_CHARS_PER_ROW) * FONT_CHAR_HEIGHT;

		// 1������`��
		Sprite_Draw(
			g_FontTextureId,
			current_x, y,
			FONT_CHAR_WIDTH * scale, FONT_CHAR_HEIGHT * scale, // �`��T�C�Y
			cut_x, cut_y,                                      // �؂蔲�����W
			FONT_CHAR_WIDTH, FONT_CHAR_HEIGHT,                 // �؂蔲���T�C�Y
			false,color
		);

		// ���̕����̕`��ʒu��X���W�����炷
		current_x += FONT_CHAR_WIDTH * scale;
	}
}
