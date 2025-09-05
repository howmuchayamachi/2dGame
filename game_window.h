/*====================================================================================

ゲームウィンドウ[game_window.h]

																Author	: Harada Ren
																Date	: 2025/06/06
--------------------------------------------------------------------------------------

======================================================================================*/
#ifndef GAME_WINDOW_H
#define GAME_WINDOW_H

#include <SDKDDKVer.h>
#define WIN32_LEAN_AND_MEAN
#include<Windows.h>

HWND GameWindow_Create(HINSTANCE hInstance);

HWND GameWindow_GetHWND();

#endif //GAME_WINDOW_H