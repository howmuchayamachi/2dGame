/*==============================================================================

  タイトルデバッグ用テキスト [debug_font.cpp]
														 Author : Harada Ren
														 Date   : 2025/08/05
--------------------------------------------------------------------------------

==============================================================================*/
#include "debug_font.h"
#include "texture.h"
#include "sprite.h"
#include <string>

static const int FONT_CHAR_WIDTH = 32;   // フォント画像1文字あたりの横幅(ピクセル)
static const int FONT_CHAR_HEIGHT = 32;  // フォント画像1文字あたりの縦幅(ピクセル)
static const int FONT_CHARS_PER_ROW = 16; // フォント画像に横何文字並んでいるか

static int g_FontTextureId = -1; // フォント画像のテクスチャID

void DebugFont_Initialize(){
	g_FontTextureId = Texture_Load(L"resource/texture/consolab_ascii_512.png"); 
}

void DebugFont_Finalize(){
}

void DebugFont_Draw(const char* text, float x, float y, const DirectX::XMFLOAT4& color, float scale){
	if (g_FontTextureId < 0) return; // テクスチャが読み込まれていなければ何もしない

	float current_x = x;
	int len = (int)strlen(text);

	// 文字列を一文字ずつループして描画
	for (int i = 0; i < len; ++i) {
		unsigned char c = text[i];

		// 改行文字の場合は、Y座標をずらしてX座標をリセット
		if (c == '\n') {
			y += FONT_CHAR_HEIGHT * scale;
			current_x = x;
			continue;
		}

		// ASCIIコードを元に、フォント画像のどこを切り抜くか計算
		int char_index = (int)c;
		int cut_x = (char_index % FONT_CHARS_PER_ROW) * FONT_CHAR_WIDTH;
		int cut_y = (char_index / FONT_CHARS_PER_ROW) * FONT_CHAR_HEIGHT;

		// 1文字を描画
		Sprite_Draw(
			g_FontTextureId,
			current_x, y,
			FONT_CHAR_WIDTH * scale, FONT_CHAR_HEIGHT * scale, // 描画サイズ
			cut_x, cut_y,                                      // 切り抜き座標
			FONT_CHAR_WIDTH, FONT_CHAR_HEIGHT,                 // 切り抜きサイズ
			false,color
		);

		// 次の文字の描画位置にX座標をずらす
		current_x += FONT_CHAR_WIDTH * scale;
	}
}
