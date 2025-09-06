/*====================================================================================

メイン処理[main.cpp]

																Author	: Harada Ren
																Date	: 2025/06/06
--------------------------------------------------------------------------------------

======================================================================================*/
#include <SDKDDKVer.h>
#define WIN32_LEAN_AND_MEAN //コンパイル時間が早くなる(古いプログラムを使わない)
#include <Windows.h>
#include "game_window.h"
#include "direct3d.h"
#include "shader.h"
#include "sprite.h"
#include "Texture.h"
#include "debug_ostream.h"
#include "sprite_anim.h"
#include "fade.h"
#include "collision.h"
#include "debug_text.h"
#include <sstream>
#include "system_timer.h"
#include "Key_logger.h"
#include "mouse.h"
#include <Xinput.h>
#pragma comment(lib,"xinput.lib")
#include "scene.h"
#include "Audio.h"
#include "game.h"

#include "polygon.h"



//メイン
int APIENTRY WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int nCmdShow) {

	//COMライブラリの初期化
	(void)CoInitializeEx(nullptr, COINIT_MULTITHREADED);

	////GPIスケーリング
	SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

	HWND hWnd = GameWindow_Create(hInstance);

	SystemTimer_Initialize();
	KeyLogger_Initialize();
	Mouse_Initialize(hWnd);
	InitAudio();

	Direct3D_Initialize(hWnd); //ダイレクト3Dの初期化
	Shader_Initialize(Direct3D_GetDevice(), Direct3D_GetContext()); //↓ダイレクト3Dの初期化の後に初期化
	Texture_Initialize(Direct3D_GetDevice(), Direct3D_GetContext());
	Sprite_Initialize(Direct3D_GetDevice(), Direct3D_GetContext());
	Fade_Initialize();
	Scene_Initialize();

	Polygon_Initialize(Direct3D_GetDevice(), Direct3D_GetContext());

#if defined(DEBUG)||defined(_DEBUG)
	hal::DebugText dt(Direct3D_GetDevice(), Direct3D_GetContext(),
		L"resource/texture/consolab_ascii_512.png",
		Direct3D_GetBackBufferWidth(), Direct3D_GetBackBufferHeight(),
		0.0f, 0.0f, //調整
		0, 0,
		0.0f, 16.0f); //調整

	Collision_DebugInitialize(Direct3D_GetDevice(), Direct3D_GetContext());
#endif

	ShowWindow(hWnd, nCmdShow); //指定のウィンドウを画面に表示する
	UpdateWindow(hWnd); //ウィンドウの中を更新する

	//fps・実行フレーム速度計測用
	double exec_last_time = SystemTimer_GetTime();
	double fps_last_time = exec_last_time;
	double current_time = 0.0;
	ULONG frame_count = 0;
	double fps = 0.0;

	//ゲームループ&メッセージループ
	MSG msg;

	//ウィンドウプログラムの中心	
	do {
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) { //ウィンドウメッセージが来ていたら
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else { //ゲームの処理

			//fps計測
			current_time = SystemTimer_GetTime(); //システム時刻を取得
			double elapsed_time = current_time - fps_last_time; //fps計測用の経過時間を計算

			if (elapsed_time >= 1.0) { //1秒ごとに計測
				fps = frame_count / elapsed_time;
				fps_last_time = current_time; //FPSを測定した時刻を保存
				frame_count = 0; //カウントをクリア
			}

			//1/60秒ごとに実行
			elapsed_time = current_time - exec_last_time;
			//if(elapsed_time>=(1.0/60.0)){ //60fpsに固定
			if (true) { //どんなfps値でもアニメーションの動きは固定
				exec_last_time = current_time; //処理した時刻を保存

				//ゲームの更新
				KeyLogger_Update();

				Mouse_State ms{};
				Mouse_GetState(&ms); //マウスの状態取得

				
				Scene_Update(elapsed_time);
				
				SpriteAnim_Update(elapsed_time);//経過時間でゲームを作る
				Fade_Update(elapsed_time);

				//ゲームの描画
				//画面のクリア
				Direct3D_Clear();
				Sprite_Begin();				
				Scene_Draw();
				Fade_Draw();

#if defined(DEBUG)||defined(_DEBUG)//デバッグの時だけ有効
				std::stringstream ss;
				//ss << "fps:" << fps << std::endl;

				dt.SetText(ss.str().c_str());
				//dt.SetText("debug!\nwatashidayo! \n nye!!", { 0.0f,0.0f,1.0f,1.0f });

				dt.Draw();
				dt.Clear();
#endif

				//画面のスワップ
				Direct3D_Present();

				frame_count++;
			}
		}
	} while (msg.message != WM_QUIT);

	
#if defined(DEBUG)||defined(_DEBUG)
	Collision_DebugFinalize();
#endif

	//初期化と逆順に後片付け
	Scene_Finalize();
	Fade_Finalize();
	Polygon_Finalize();
	Shader_Finalize();
	Sprite_Finalize();
	Texture_Finalize();
	Direct3D_Finalize();
	UninitAudio();
	Mouse_Finalize();

	return (int)msg.wParam;
}

