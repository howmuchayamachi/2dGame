/*==============================================================================

  クリア画面制御[clear.h]
														 Author : Harada Ren
														 Date   : 2025/08/18
--------------------------------------------------------------------------------

==============================================================================*/
#include "clear.h"
#include "texture.h"
#include "sprite.h"
#include "Audio.h"
#include "fade.h"
#include "scene.h"
#include "key_logger.h"
#include "direct3d.h"
#include "game_window.h"
#include "DirectXMath.h"
using namespace DirectX;

static int g_ClearBackGroundTexture = -1;

static int g_GameClearTexture = -1;
static int g_GoToTitleTexture = -1;
static int g_ExitTexture = -1;

static int g_ClearBGMId = -1;
static int g_SelectAudioId = -1;
static int g_EnterAudioId = -1;

static int g_CurrentSelection = CLEAR_GOTOTITLE;

static bool g_ClearStart = false;
static bool g_ClearEnd = false;

void Clear_Initialize(){
	g_ClearBackGroundTexture = Texture_Load(L"resource/texture/bg/house_inforest.png");

	g_GameClearTexture = Texture_Load(L"resource/texture/Font/gameclear.png");
	g_GoToTitleTexture = Texture_Load(L"resource/texture/Font/gototitle.png");
	g_ExitTexture = Texture_Load(L"resource/texture/Font/exit.png");
	
	g_ClearBGMId = LoadAudio("resource/audio/bgm/gameclear.wav");
	g_SelectAudioId = LoadAudio("resource/audio/select1.wav");
	g_EnterAudioId = LoadAudio("resource/audio/enter1.wav");

	g_CurrentSelection = CLEAR_GOTOTITLE;

	g_ClearStart = false;
	g_ClearEnd = false;

	PlayAudio(g_ClearBGMId, true);
	Fade_Start(1.0, false, { 1.0f,1.0f,1.0f });
}

void Clear_Finalize(){
	StopAllAudio();
	UnloadAllAudio();
}

void Clear_Update(){
	if (!g_ClearStart && Fade_GetState() == FADE_STATE_FINISHED_IN) {
		g_ClearStart = true;		
	}

	if (g_ClearStart && Fade_GetState() != FADE_STATE_OUT) {
		if (KeyLogger_IsTrigger(KK_W) || KeyLogger_IsTrigger(KK_UP)) {
			g_CurrentSelection--;
			if (g_CurrentSelection < 0) {
				g_CurrentSelection = CLEAR_MAX - 1;
			}
			PlayAudio(g_SelectAudioId);
		}

		if (KeyLogger_IsTrigger(KK_S) || KeyLogger_IsTrigger(KK_DOWN)) {
			g_CurrentSelection++;
			if (g_CurrentSelection >= CLEAR_MAX) {
				g_CurrentSelection = 0;
			}
			PlayAudio(g_SelectAudioId);
		}

		if (KeyLogger_IsTrigger(KK_ENTER)) {
			PlayAudio(g_EnterAudioId);
			switch (g_CurrentSelection) {
			case CLEAR_GOTOTITLE:
				g_ClearEnd = true;
				Fade_Start(0.5, true);
				break;

			case CLEAR_EXIT: //ゲーム終了
				SendMessage(GameWindow_GetHWND(), WM_CLOSE, 0, 0);
				break;


			default:
				break;
			}
		}
	}

	if (g_ClearEnd && Fade_GetState() == FADE_STATE_FINISHED_OUT) {
		Scene_Change(SCENE_TITLE);
	}
}

void Clear_Draw(){
	float screen_width = (float)Direct3D_GetBackBufferWidth();
	float screen_height = (float)Direct3D_GetBackBufferHeight();

	Sprite_Draw(g_ClearBackGroundTexture, 0.0f, 0.0f, screen_width, screen_height);

	// 画面の中央座標を取得
	float center_x = screen_width / 2.0f;
	float center_y = screen_height / 2.0f;

	// 通常の色と、選択されている時の色を定義
	XMFLOAT4 color_normal = { 1.0f, 1.0f, 1.0f, 0.1f }; 
	XMFLOAT4 color_selected = { 1.0f, 1.0f, 0.1f, 1.0f };

	Sprite_Draw(g_GameClearTexture, center_x - 600.0f, center_y - 400.0f, 1000.0f, 250.0f);

	Sprite_Draw(g_GoToTitleTexture, center_x - 500.0f, center_y + 100.0f,
		(g_CurrentSelection == CLEAR_GOTOTITLE) ? color_selected : color_normal);

	// EXIT の描画
	Sprite_Draw(g_ExitTexture, center_x - 450.0f, center_y + 300.0f,
		(g_CurrentSelection == CLEAR_EXIT) ? color_selected : color_normal);
}
