/*==============================================================================

   エフェクト描画 [effect.cpp]
														 Author : Harada Ren
														 Date   : 2025/07/04
--------------------------------------------------------------------------------

==============================================================================*/
#include "effect.h"
#include "DirectXMath.h"
using namespace DirectX;
#include "texture.h"
#include "sprite_anim.h"
#include "Audio.h"
#include "audio_manager.h"
#include "map.h"


struct Effect {
	XMFLOAT2 position;
	//XMFLOAT2 velocity; //動くものを使いたかったら
	//double LifeTime;
	int sprite_anim_id;
	bool isEnable;
};


static constexpr int EFFECT_MAX = 256;
static Effect g_Effects[EFFECT_MAX]{};
static int g_EffectTexId = -1;
static int g_AnimPatternId = -1;
static int g_EffectSoundId = -1;


void Effect_Initialize(){
	for (Effect& e : g_Effects) {
		e.isEnable = false;
	}
	g_EffectTexId = Texture_Load(L"resource/texture/explosion2.png");
	g_EffectSoundId = LoadAudio("resource/audio/explosion01.wav");
	g_AnimPatternId = SpriteAnim_RegisterPattern(g_EffectTexId,7,7,0.1,{300,400},{0,0},false);
}

void Effect_Finalize(){
}

void Effect_Update(){

	for (Effect& e : g_Effects) {
		if (!e.isEnable) continue;

		if (SpriteAnim_IsStopped(e.sprite_anim_id)) {
			SpriteAnim_DestroyPlayer(e.sprite_anim_id);
			e.isEnable = false;
		}
	}
}

void Effect_Draw(){
	for (Effect& e : g_Effects) {
		//使っていなかったら読み飛ばす
		if (!e.isEnable) continue;
		// カメラのオフセットを取得
		XMFLOAT2 offset = Map_GetWorldOffset();

		// プレイヤーのワールド座標からスクリーン座標を計算
		float screen_x = e.position.x - offset.x;
		float screen_y = e.position.y - offset.y;

		SpriteAnim_Draw(e.sprite_anim_id, screen_x, screen_y, 256.0f, 256.0f);
	}
}

void Effect_Create(const DirectX::XMFLOAT2& position){
	for (Effect& e : g_Effects) {
		if (e.isEnable) continue;

		//空き領域発見して1発分作成
		e.isEnable = true;
		e.position = position;
		e.sprite_anim_id = SpriteAnim_CreatePlayer(g_AnimPatternId);
		PlayAudio(g_EffectSoundId);
		break;
	}
}
