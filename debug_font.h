/*==============================================================================

  タイトルデバッグ用テキスト [debug_font.h]
														 Author : Harada Ren
														 Date   : 2025/08/05
--------------------------------------------------------------------------------

==============================================================================*/
#ifndef DEBUG_FONT_H
#define DEBUG_FONT_H

#include <DirectXMath.h>

void DebugFont_Initialize();
void DebugFont_Finalize();

// 文字列を描画する関数の宣言
void DebugFont_Draw(
	const char* text,      // 描画したい文字列
	float x,               // 描画開始位置 X座標
	float y,               // 描画開始位置 Y座標
	const DirectX::XMFLOAT4& color = { 1.0f, 1.0f, 1.0f, 1.0f }, // 文字の色
	float scale = 1.0f     // 文字の拡大率
);

#endif //DEBUG_FONT_H