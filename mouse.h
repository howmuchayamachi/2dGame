/*====================================================================================

マウス入力の記録[mouse.h]

																Author	: Harada Ren
																Date	: 2025/06/27
--------------------------------------------------------------------------------------

======================================================================================*/
#ifndef MOUSE_H
#define MOUSE_H
#pragma once


#include <windows.h>
#include <memory>


// マウスモード
typedef enum Mouse_PositionMode_tag
{
	MOUSE_POSITION_MODE_ABSOLUTE, // 絶対座標モード
	MOUSE_POSITION_MODE_RELATIVE, // 相対座標モード
} Mouse_PositionMode;


// マウス状態構造体
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


// マウスモジュールの初期化
void Mouse_Initialize(HWND window);

// マウスモジュールの終了処理
void Mouse_Finalize(void);

// マウスの状態を取得する
void Mouse_GetState(Mouse_State* pState);

// 累積したマウススクロールホイール値をリセットする
void Mouse_ResetScrollWheelValue(void);

// マウスのポジションモードを設定する（デフォルトは絶対座標モード）
void Mouse_SetMode(Mouse_PositionMode mode);

// マウスの接続を検出する
bool Mouse_IsConnected(void);

// マウスカーソルが表示されているか確認する
bool Mouse_IsVisible(void);

// マウスカーソル表示を設定する
void Mouse_SetVisible(bool visible);

// マウス制御のためのウィンドウメッセージプロシージャフック関数
void Mouse_ProcessMessage(UINT message, WPARAM wParam, LPARAM lParam);


#endif // MOUSE_H
