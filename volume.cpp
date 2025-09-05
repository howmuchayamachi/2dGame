/*==============================================================================

  ボリューム制御[volume.h]
														 Author : Harada Ren
														 Date   : 2025/08/20
--------------------------------------------------------------------------------

==============================================================================*/
#include "volume.h"
#include "texture.h"
#include "sprite.h"
#include "key_logger.h"
#include "direct3d.h"
#include "Audio.h"
#include "audio_manager.h"
#include "scene.h"
#include <DirectXMath.h>
using namespace DirectX;

static int g_VolumeTexture = -1;
static int g_BackTexture = -1;
static int g_VolumeBarFrameTexture = -1;
static int g_VolumeBarTexture = -1;

static int g_DownButtonAbeforeTexture = -1;
static int g_DownButtonAafterTexture = -1;
static int g_DownButtonArrowbeforeTexture = -1;
static int g_DownButtonArrowafterTexture = -1;

static int g_UpButtonDbeforeTexture = -1;
static int g_UpButtonDafterTexture = -1;
static int g_UpButtonArrowbeforeTexture = -1;
static int g_UpButtonArrowafterTexture = -1;

static int g_ReturnExplanationTexture = -1;

static int g_SelectAudioId = -1;
static int g_MenuEndAudioId = -1;

static double g_accumulated_time = 0.0;

void Volume_Initialize(){
	g_VolumeTexture = Texture_Load(L"resource/texture/Font/volume.png");
	g_BackTexture = Texture_Load(L"resource/texture/white.png");
	g_VolumeBarFrameTexture = Texture_Load(L"resource/texture/ui/hp_frame.png");
	g_VolumeBarTexture = Texture_Load(L"resource/texture/ui/hp.png");

	g_DownButtonAbeforeTexture = Texture_Load(L"resource/texture/ui/button_A_bef.png");
	g_DownButtonAafterTexture = Texture_Load(L"resource/texture/ui/button_A_aft.png");
	g_DownButtonArrowbeforeTexture = Texture_Load(L"resource/texture/ui/button_left_bef.png");
	g_DownButtonArrowafterTexture = Texture_Load(L"resource/texture/ui/button_left_aft.png");

	g_UpButtonDbeforeTexture = Texture_Load(L"resource/texture/ui/button_D_bef.png");
	g_UpButtonDafterTexture = Texture_Load(L"resource/texture/ui/button_D_aft.png");
	g_UpButtonArrowbeforeTexture = Texture_Load(L"resource/texture/ui/button_right_bef.png");
	g_UpButtonArrowafterTexture = Texture_Load(L"resource/texture/ui/button_right_aft.png");

	g_ReturnExplanationTexture = Texture_Load(L"resource/texture/Font/explanation_escape_enter.png");

	g_SelectAudioId = LoadAudio("resource/audio/select1.wav");
	g_MenuEndAudioId = LoadAudio("resource/audio/menuend.wav");

	g_accumulated_time = 0.0;
}

void Volume_Finalize(){
}

void Volume_Update(double elapsed_time){
	g_accumulated_time += elapsed_time;
	float current_volume = AudioManager_GetVolume();
	//音量を下げる
	if (KeyLogger_IsPressed(KK_A) || KeyLogger_IsPressed(KK_LEFT)) {
		AudioManager_SetVolume(current_volume - 0.005f);
		Audio_UpdateBGMVolume();
		if (g_accumulated_time > 0.1) {
			PlayAudio(g_SelectAudioId);
			g_accumulated_time = 0.0;
		}
	}
	//音量を上げる
	if (KeyLogger_IsPressed(KK_D) || KeyLogger_IsPressed(KK_RIGHT)) {
		AudioManager_SetVolume(current_volume + 0.005f);
		Audio_UpdateBGMVolume();
		if (g_accumulated_time > 0.1) {
			PlayAudio(g_SelectAudioId);
			g_accumulated_time = 0.0;
		}
	}

	if (KeyLogger_IsTrigger(KK_ESCAPE) || KeyLogger_IsTrigger(KK_ENTER)) {
		PlayAudio(g_MenuEndAudioId);
		Scene_Pop();
	}
}

void Volume_Draw(){
	// 画面の中央座標を取得
	float center_x = Direct3D_GetBackBufferWidth() / 2.0f;
	float center_y = Direct3D_GetBackBufferHeight() / 2.0f;

	Sprite_Draw(g_BackTexture, 0.0f, 0.0f, (float)Direct3D_GetBackBufferWidth(), (float)Direct3D_GetBackBufferHeight());

	Sprite_Draw(g_VolumeTexture, center_x - 500.0f, center_y - 400.0f, 1000.0f, 250.0f,{0.0f,0.0f,0.0f,1.0f});

	Sprite_Draw(g_VolumeBarFrameTexture, center_x - 500.0f, center_y, 1000.0f, 100.0f);

	Sprite_Draw(g_VolumeBarTexture, center_x - 500.0f, center_y, 1000.0f * AudioManager_GetVolume(), 100.0f);

	//ガイド表示
	if (KeyLogger_IsPressed(KK_A) || KeyLogger_IsPressed(KK_LEFT)) {
		Sprite_Draw(g_DownButtonAafterTexture, center_x - 600.0f, center_y + 100.0f, 100.0f, 100.0f);
		Sprite_Draw(g_DownButtonArrowafterTexture, center_x - 500.0f, center_y + 100.0f, 100.0f, 100.0f);
	}
	else {
		Sprite_Draw(g_DownButtonAbeforeTexture, center_x - 600.0f, center_y + 100.0f, 100.0f, 100.0f);
		Sprite_Draw(g_DownButtonArrowbeforeTexture, center_x - 500.0f, center_y + 100.0f, 100.0f, 100.0f);
	} 
	
	if (KeyLogger_IsPressed(KK_D) || KeyLogger_IsPressed(KK_RIGHT)) {
		Sprite_Draw(g_UpButtonDafterTexture, center_x + 400.0f, center_y + 100.0f, 100.0f, 100.0f);
		Sprite_Draw(g_UpButtonArrowafterTexture, center_x + 500.0f, center_y + 100.0f, 100.0f, 100.0f);
	}
	else {
		Sprite_Draw(g_UpButtonDbeforeTexture, center_x + 400.0f, center_y + 100.0f, 100.0f, 100.0f);
		Sprite_Draw(g_UpButtonArrowbeforeTexture, center_x + 500.0f, center_y + 100.0f, 100.0f, 100.0f);
	}

	//エンターキーとエスケープキーで戻る
	Sprite_Draw(g_ReturnExplanationTexture, center_x - 400.0f, center_y + 300.0f, 800.0f, 100.0f);
}
