/*==============================================================================

  リザルト制御[result.cpp]
														 Author : Harada Ren
														 Date   : 2025/07/11
--------------------------------------------------------------------------------

==============================================================================*/
#include "result.h"
#include "texture.h"
#include "sprite.h"
#include "key_logger.h"
#include "direct3d.h"
#include "scene.h"
#include "fade.h"
#include "Audio.h"
#include "audio_manager.h"
#include "game_window.h"
#include "DirectXMath.h"
using namespace DirectX;

static int g_ResultBgTexture = -1;

static int g_GameOverTexture = -1;
static int g_ContinueTexture = -1;
static int g_GoToTitleTexture = -1;
static int g_ExitTexture = -1;

static int g_GameOverAudioId = -1;
static int g_SelectAudioId = -1;
static int g_EnterAudioId = -1;

static int g_CurrentSelection = RESULT_CONTINUE;

static bool g_ResultStart = false;
static bool g_ResultEnd = false;


void Result_Initialize(){
	g_ResultBgTexture = Texture_Load(L"resource/texture/bg/dark_forest.png");

	g_GameOverTexture = Texture_Load(L"resource/texture/Font/gameover.png");
	g_ContinueTexture = Texture_Load(L"resource/texture/Font/continue.png");
	g_GoToTitleTexture = Texture_Load(L"resource/texture/Font/gototitle.png");
	g_ExitTexture = Texture_Load(L"resource/texture/Font/exit.png");

	g_GameOverAudioId = LoadAudio("resource/audio/bgm/gameover.wav");
	g_SelectAudioId = LoadAudio("resource/audio/select1.wav");
	g_EnterAudioId = LoadAudio("resource/audio/enter1.wav");

	g_CurrentSelection = RESULT_CONTINUE;

	g_ResultStart = false;
	g_ResultEnd = false;

	PlayAudio(g_GameOverAudioId, true);
	Fade_Start(0.5, false);
}

void Result_Finalize(){
	StopAllAudio();
	UnloadAllAudio();
}

void Result_Update(){
	if (!g_ResultStart && Fade_GetState() == FADE_STATE_FINISHED_IN) {
		g_ResultStart = true;
	}

	if (g_ResultStart && Fade_GetState() != FADE_STATE_OUT) {
		if (KeyLogger_IsTrigger(KK_W) || KeyLogger_IsTrigger(KK_UP)) {
			g_CurrentSelection--;
			if (g_CurrentSelection < 0) {
				g_CurrentSelection = RESULT_MAX - 1;
			}
			PlayAudio(g_SelectAudioId);
		}

		if (KeyLogger_IsTrigger(KK_S) || KeyLogger_IsTrigger(KK_DOWN)) {
			g_CurrentSelection++;
			if (g_CurrentSelection >= RESULT_MAX) {
				g_CurrentSelection = 0;
			}
			PlayAudio(g_SelectAudioId);
		}

		if (KeyLogger_IsTrigger(KK_ENTER)) {
			PlayAudio(g_EnterAudioId);
			switch (g_CurrentSelection) {
			case RESULT_CONTINUE: //ゲームスタート
				Fade_Start(1.0, true);
				g_ResultEnd = true;
				break;

			case RESULT_GOTOTITLE:
				Fade_Start(0.5, true);
				break;

			case RESULT_EXIT: //ゲーム終了
				SendMessage(GameWindow_GetHWND(), WM_CLOSE, 0, 0);
				break;

			default:
				break;
			}
		}
	}

	if (g_ResultEnd && Fade_GetState() == FADE_STATE_FINISHED_OUT) {
		Scene_Change(SCENE_GAME);
	}
	else if (!g_ResultEnd && Fade_GetState() == FADE_STATE_FINISHED_OUT) {
		Scene_Change(SCENE_TITLE);
	}
}

void Result_Draw(){
	float screen_width = (float)Direct3D_GetBackBufferWidth();
	float screen_height = (float)Direct3D_GetBackBufferHeight();

	Sprite_Draw(g_ResultBgTexture,0.0f,0.0f, screen_width, screen_height);

	// 画面の中央座標を取得
	float center_x = screen_width / 2.0f;
	float center_y = screen_height / 2.0f;

	// 通常の色と、選択されている時の色を定義
	XMFLOAT4 color_normal = { 1.0f, 1.0f, 1.0f, 0.2f }; // 白色
	XMFLOAT4 color_selected = { 1.0f, 1.0f, 1.0f, 1.0f }; // 黄色

	Sprite_Draw(g_GameOverTexture, center_x - 600.0f, center_y - 400.0f, 1000.0f, 250.0f);

	// GAMESTART の描画
	Sprite_Draw(g_ContinueTexture, center_x - 500.0f, center_y-100.0f,
		(g_CurrentSelection == RESULT_CONTINUE) ? color_selected : color_normal);

	Sprite_Draw(g_GoToTitleTexture, center_x - 500.0f, center_y + 100.0f,
		(g_CurrentSelection == RESULT_GOTOTITLE) ? color_selected : color_normal);

	// EXIT の描画
	Sprite_Draw(g_ExitTexture, center_x - 500.0f, center_y + 300.0f,
		(g_CurrentSelection == RESULT_EXIT) ? color_selected : color_normal);
}
