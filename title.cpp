/*==============================================================================

  タイトル制御[title.cpp]
														 Author : Harada Ren
														 Date   : 2025/07/11
--------------------------------------------------------------------------------

==============================================================================*/
#include "title.h"
#include "texture.h"
#include "sprite.h"
#include "key_logger.h"
#include "direct3d.h"
#include "DirectXMath.h"
using namespace DirectX;
#include "game_window.h"
#include "scene.h"
#include "fade.h"
#include "Audio.h"
#include "audio_manager.h"


static int g_TitleBgTexture = -1;

static int g_TitleTexture = -1;
static int g_GameStartTexture = -1;
static int g_VolumeTexture = -1;
static int g_ExitTexture = -1;

static bool g_TitleStart = false;
static bool g_TitleEnd = false;

static int g_CurrentSelection = TITLE_GAMESTART;

static int g_TitleBGMId = -1;
static int g_SelectAudioId = -1;
static int g_EnterAudioId = -1;

static double g_accumulated_time = 0.0;

void Title_Initialize(){
	g_TitleBgTexture = Texture_Load(L"resource/texture/bg/title_forest.png");
	g_TitleTexture = Texture_Load(L"resource/texture/Font/intheforest.png");
	g_GameStartTexture = Texture_Load(L"resource/texture/Font/gamestart.png");
	g_VolumeTexture= Texture_Load(L"resource/texture/Font/volume.png");
	g_ExitTexture = Texture_Load(L"resource/texture/Font/exit.png");

	g_TitleStart = false;
	g_TitleEnd = false;

	g_TitleBGMId= LoadAudio("resource/audio/bgm/title.wav");
	g_SelectAudioId = LoadAudio("resource/audio/select1.wav");
	g_EnterAudioId = LoadAudio("resource/audio/enter1.wav");

	g_CurrentSelection = TITLE_GAMESTART;

	g_accumulated_time = 0.0f;

	PlayAudio(g_TitleBGMId, true);
	Fade_Start(0.5, false, { 0.0f,0.0f,0.0f });
}

void Title_Finalize(){
	StopAllAudio();
	UnloadAllAudio();
}

void Title_Update(){
	if (!g_TitleStart && Fade_GetState() == FADE_STATE_FINISHED_IN) {
		g_TitleStart = true;
	}

	if (g_TitleStart && Fade_GetState() != FADE_STATE_OUT) {
		if (KeyLogger_IsTrigger(KK_W) || KeyLogger_IsTrigger(KK_UP)) {
			g_CurrentSelection--;
			if (g_CurrentSelection < 0) {
				g_CurrentSelection = TITLE_MAX - 1;
			}
			PlayAudio(g_SelectAudioId);
		}

		if (KeyLogger_IsTrigger(KK_S) || KeyLogger_IsTrigger(KK_DOWN)) {
			g_CurrentSelection++;
			if (g_CurrentSelection >= TITLE_MAX) {
				g_CurrentSelection = 0;
			}
			PlayAudio(g_SelectAudioId);
		}

		if (KeyLogger_IsTrigger(KK_ENTER)) {
			PlayAudio(g_EnterAudioId);
			switch (g_CurrentSelection) {
			case TITLE_GAMESTART: //ゲームスタート
				Fade_Start(1.0, true,{1.0f,1.0f,1.0f});
				g_TitleEnd = true;
				break;

			case TITLE_VOLUME:
				Scene_Push(SCENE_VOLUME);
				break;

			case TITLE_EXIT: //ゲーム終了
				SendMessage(GameWindow_GetHWND(), WM_CLOSE, 0, 0);
				break;

			default:
				break;
			}
		}
	}

	if (g_TitleEnd && Fade_GetState() == FADE_STATE_FINISHED_OUT) {
		Scene_Change(SCENE_GAME);
	}
}

void Title_Draw(){

	float screen_width = (float)Direct3D_GetBackBufferWidth();
	float screen_height = (float)Direct3D_GetBackBufferHeight();

	//背景
	Sprite_Draw(g_TitleBgTexture, 0.0f, 0.0f, screen_width, screen_height);

	// 画面の中央座標を取得
	float center_x = Direct3D_GetBackBufferWidth() / 2.0f;
	float center_y = Direct3D_GetBackBufferHeight() / 2.0f;

	//タイトル
	Sprite_Draw(g_TitleTexture, center_x - 800, center_y - 400,1500.0f,1500.0f/4.0f);

	// 通常の色と、選択されている時の色を定義
	XMFLOAT4 color_normal = { 1.0f, 1.0f, 1.0f, 0.2f }; // 白色
	XMFLOAT4 color_selected = { 1.0f, 1.0f, 0.1f, 1.0f }; // 黄色

	// GAMESTART の描画
	Sprite_Draw(g_GameStartTexture, center_x -500.0f, center_y,
		(g_CurrentSelection == TITLE_GAMESTART) ? color_selected : color_normal);

	//音量バー表示
	Sprite_Draw(g_VolumeTexture, center_x - 500.0f, center_y + 150.0f,
		(g_CurrentSelection == TITLE_VOLUME) ? color_selected : color_normal);


	// EXIT の描画
	Sprite_Draw(g_ExitTexture, center_x-500.0f, center_y+300.0f,
		(g_CurrentSelection == TITLE_EXIT) ? color_selected : color_normal);
}
