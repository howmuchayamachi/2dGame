/*==============================================================================

  ポーズ画面制御[pause.h]
														 Author : Harada Ren
														 Date   : 2025/08/20
--------------------------------------------------------------------------------

==============================================================================*/
#include "pause.h"
#include "key_logger.h"
#include "direct3d.h"
#include <DirectXMath.h>
using namespace DirectX;
#include "texture.h"
#include "sprite.h"
#include "Audio.h"
#include "audio_manager.h"
#include "fade.h"
#include "scene.h"
#include "game.h"

static int g_BackTexture = -1;
static int g_PauseTexture = -1;
static int g_BackToGameTexture = -1;
static int g_VolumeTexture = -1;
static int g_GoToTitleTexture = -1;

static int g_CurrentSelection = PAUSE_BACKTOGAME;

static int g_SelectAudioId = -1;
static int g_EnterAudioId = -1;
static int g_MenuEndAudioId = -1;

static double g_accumulated_time = 0.0;

static bool g_IsGamePaused = false;

void Pause_Initialize(){
	g_CurrentSelection = PAUSE_BACKTOGAME;

	g_BackTexture = Texture_Load(L"resource/texture/white.png");
	g_PauseTexture = Texture_Load(L"resource/texture/Font/pause.png");
	g_BackToGameTexture = Texture_Load(L"resource/texture/Font/backtogame.png");
	g_VolumeTexture = Texture_Load(L"resource/texture/Font/volume.png");
	g_GoToTitleTexture = Texture_Load(L"resource/texture/Font/gototitle.png");

	g_SelectAudioId = LoadAudio("resource/audio/select1.wav");
	g_EnterAudioId = LoadAudio("resource/audio/enter1.wav");
	g_MenuEndAudioId = LoadAudio("resource/audio/menuend.wav");

	g_accumulated_time = 0.0;
}

void Pause_Finalize(){
}

void Pause_Update(){
	if (Fade_GetState() != FADE_STATE_OUT) {
		if (KeyLogger_IsTrigger(KK_W) || KeyLogger_IsTrigger(KK_UP)) {
			g_CurrentSelection--;
			if (g_CurrentSelection < 0) {
				g_CurrentSelection = PAUSE_MAX - 1;
			}
			PlayAudio(g_SelectAudioId);
		}

		if (KeyLogger_IsTrigger(KK_S) || KeyLogger_IsTrigger(KK_DOWN)) {
			g_CurrentSelection++;
			if (g_CurrentSelection >= PAUSE_MAX) {
				g_CurrentSelection = 0;
			}
			PlayAudio(g_SelectAudioId);
		}

		if (KeyLogger_IsTrigger(KK_ENTER)) {
			switch (g_CurrentSelection) {
			case PAUSE_BACKTOGAME:
				Game_SetPause(false);
				PlayAudio(g_MenuEndAudioId);
				Scene_Pop();
				break;

			case PAUSE_VOLUME:
				PlayAudio(g_EnterAudioId);
				Scene_Push(SCENE_VOLUME);
				break;

			case PAUSE_GOTOTITLE:
				PlayAudio(g_EnterAudioId);
				Fade_Start(1.0, true);
				break;

			default:
				break;
			}
		}

		if (KeyLogger_IsTrigger(KK_ESCAPE)) {
			Game_SetPause(false);
			PlayAudio(g_MenuEndAudioId);
			Scene_Pop();
		}
	}

	if (Fade_GetState() == FADE_STATE_FINISHED_OUT) {
		Game_SetPause(false);
		Scene_Change(SCENE_TITLE);
	}
}

void Pause_Draw(){
	// 画面の中央座標を取得
	float center_x = Direct3D_GetBackBufferWidth() / 2.0f;
	float center_y = Direct3D_GetBackBufferHeight() / 2.0f;

	// 通常の色と、選択されている時の色を定義
	XMFLOAT4 color_normal = { 0.0f, 0.0f, 0.0f, 0.1f };
	XMFLOAT4 color_selected = { 0.0f, 0.0f, 0.0f, 1.0f };

	//半透明な背景
	Sprite_Draw(g_BackTexture, 0.0f, 0.0f, (float)Direct3D_GetBackBufferWidth(), (float)Direct3D_GetBackBufferHeight(), { 1.0f,1.0f,1.0f,0.8f });

	
	Sprite_Draw(g_PauseTexture, center_x - 550.0f, center_y - 400.0f, 1000.0f, 250.0f);

	Sprite_Draw(g_BackToGameTexture, center_x - 550.0f, center_y-100.0f,
		(g_CurrentSelection == PAUSE_BACKTOGAME) ? color_selected : color_normal);

	Sprite_Draw(g_VolumeTexture, center_x - 500, center_y + 100,
		(g_CurrentSelection == PAUSE_VOLUME) ? color_selected : color_normal);

	Sprite_Draw(g_GoToTitleTexture, center_x - 500.0f, center_y + 300.0f,
		(g_CurrentSelection == PAUSE_GOTOTITLE) ? color_selected : color_normal);
}

void Game_SetPause(bool is_paused) {
	g_IsGamePaused = is_paused;
}

bool Game_IsPaused() {
	return g_IsGamePaused;
}