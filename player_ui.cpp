/*====================================================================================

プレイヤーUI管理[player_ui.cpp]

																Author	: Harada Ren
																Date	: 2025/08/07
--------------------------------------------------------------------------------------

======================================================================================*/
#include "player_ui.h"
#include "texture.h"
#include "sprite.h"
#include "Player.h"
#include "direct3d.h"
#include <DirectXMath.h>
using namespace DirectX;
#include "key_logger.h"

static int g_HpBarTexId = -1;
static int g_HpRedTexId = -1;
static int g_HpFrameTexId = -1;
static int g_MpBarTexId = -1;

static int g_HpCharTexId = -1;
static int g_MpCharTexId = -1;

static int g_WhiteTexId = -1;

static int g_KeyAbefore_TexId = -1;
static int g_KeyAafter_TexId = -1;
static int g_KeyDbefore_TexId = -1;
static int g_KeyDafter_TexId = -1;
static int g_KeySbefore_TexId = -1;
static int g_KeySafter_TexId = -1;
static int g_KeyJbefore_TexId = -1;
static int g_KeyJafter_TexId = -1;
static int g_KeySPACEbefore_TexId = -1;
static int g_KeySPACEafter_TexId = -1;
static int g_KeyPLUSbefore_TexId = -1;
static int g_KeyESCAPEbefore_TexId = -1;
static int g_KeySHIFTbefore_TexId = -1;
static int g_KeySHIFTafter_TexId = -1;
static int g_KeyTABbefore_TexId = -1;


static int g_HoldTexId = -1;
static int g_ReleaseTexId = -1;
static int g_Explanation_Move_TexId = -1;
static int g_Explanation_Attack_TexId = -1;
static int g_Explanation_StrongAttack_TexId = -1;
static int g_Explanation_Bullet_TexId = -1;
static int g_Explanation_Jump_TexId = -1;
static int g_Explanation_Getoff_TexId = -1;
static int g_Explanation_Pause_TexId = -1;
static int g_Explanation_Dash_TexId = -1;
static int g_Explanation_Swicth_TexId = -1;


static constexpr float KEY_WIDTH = 30.0f;
static constexpr float KEY_HEIGHT = 30.0f;

static constexpr float KEY_FARLEFT_WIDTH = 20.0f;
static constexpr float KEY_TOP_HEIGHT = 20.0f;

static constexpr float EXPLANATION_HEIGHT = 20.0f;

//後から追いつく赤いHPバー
static float g_HpRedValue = 0.0f;
static float g_MpRedValue = 0.0f;

static bool g_isHpDecrease = false;
static bool g_isMpDecrease = false;

static bool g_isDisplay = true;

void PlayerUI_Initialize() {
	g_HpBarTexId = Texture_Load(L"resource/texture/ui/hp.png");
	g_HpRedTexId = Texture_Load(L"resource/texture/ui/hp_red.png");
	g_HpFrameTexId = Texture_Load(L"resource/texture/ui/hp_frame.png");
	g_MpBarTexId = Texture_Load(L"resource/texture/ui/mp.png");

	g_HpCharTexId = Texture_Load(L"resource/texture/Font/HP.png");
	g_MpCharTexId = Texture_Load(L"resource/texture/Font/MP.png");

	g_WhiteTexId = Texture_Load(L"resource/texture/white.png");

	g_KeyAbefore_TexId = Texture_Load(L"resource/texture/ui/button_A_bef.png");
	g_KeyAafter_TexId = Texture_Load(L"resource/texture/ui/button_A_aft.png");
	g_KeyDbefore_TexId = Texture_Load(L"resource/texture/ui/button_D_bef.png");
	g_KeyDafter_TexId = Texture_Load(L"resource/texture/ui/button_D_aft.png");
	g_KeySbefore_TexId = Texture_Load(L"resource/texture/ui/button_S_bef.png");
	g_KeySafter_TexId = Texture_Load(L"resource/texture/ui/button_S_aft.png");
	g_KeyJbefore_TexId = Texture_Load(L"resource/texture/ui/button_J_bef.png");
	g_KeyJafter_TexId = Texture_Load(L"resource/texture/ui/button_J_aft.png");
	g_KeySPACEbefore_TexId = Texture_Load(L"resource/texture/ui/button_Space_bef.png");
	g_KeySPACEafter_TexId = Texture_Load(L"resource/texture/ui/button_Space_aft.png");
	g_KeyPLUSbefore_TexId = Texture_Load(L"resource/texture/ui/button_Plus_bef.png");
	g_KeyESCAPEbefore_TexId = Texture_Load(L"resource/texture/ui/button_Escape_bef.png");
	g_KeySHIFTbefore_TexId = Texture_Load(L"resource/texture/ui/button_Shift_bef.png");
	g_KeySHIFTafter_TexId = Texture_Load(L"resource/texture/ui/button_Shift_aft.png");
	g_KeyTABbefore_TexId = Texture_Load(L"resource/texture/ui/button_Tab_bef.png");

	g_HoldTexId = Texture_Load(L"resource/texture/Font/hold.png");
	g_ReleaseTexId = Texture_Load(L"resource/texture/Font/RELEASE.png");

	g_Explanation_Move_TexId = Texture_Load(L"resource/texture/Font/explanation_move.png");
	g_Explanation_Attack_TexId = Texture_Load(L"resource/texture/Font/explanation_attack.png");
	g_Explanation_StrongAttack_TexId = Texture_Load(L"resource/texture/Font/explanation_strongattack.png");
	g_Explanation_Bullet_TexId = Texture_Load(L"resource/texture/Font/explanation_bullet.png");
	g_Explanation_Jump_TexId = Texture_Load(L"resource/texture/Font/explanation_jump.png");
	g_Explanation_Getoff_TexId = Texture_Load(L"resource/texture/Font/explanation_getoff.png");
	g_Explanation_Pause_TexId = Texture_Load(L"resource/texture/Font/explanation_pause.png");
	g_Explanation_Dash_TexId = Texture_Load(L"resource/texture/Font/explanation_dash.png");
	g_Explanation_Swicth_TexId = Texture_Load(L"resource/texture/Font/explanation_swicth.png");


	g_HpRedValue = Player_MAXHP;
	g_MpRedValue = Player_MAXMP;

	g_isHpDecrease = false;
	g_isMpDecrease = false;

	g_isDisplay = true;
}

void PlayerUI_Finalize() {
}

void PlayerUI_Update(double elapsed_time) {
	//プレイヤーの現在のHPを取得
	float current_hp = Player_GetHp();

	//赤いHPバーが本当のHPよりも大きい場合
	if (g_HpRedValue > current_hp) {
		g_isHpDecrease = true;

		//少しずつ赤いHPバーを減らす
		g_HpRedValue -= 0.5f * (float)elapsed_time;
		if (g_HpRedValue < current_hp) {
			g_HpRedValue = current_hp;
		}
	}
	else {
		g_HpRedValue = current_hp;
		g_isHpDecrease = false;
	}

	float current_mp = (float)Player_GetMp();
	if (g_MpRedValue > current_mp) {
		g_isMpDecrease = true;

		g_MpRedValue -= 2.0f * (float)elapsed_time;
		if (g_MpRedValue < current_mp) {
			g_MpRedValue = current_mp;
		}
	}
	else {
		g_MpRedValue = current_mp;
		g_isMpDecrease = false;
	}

	if (KeyLogger_IsTrigger(KK_TAB)) {
		PlayerUI_Switch();
	}

}

void PlayerUI_Draw() {
	float screen_width = (float)Direct3D_GetBackBufferWidth();

	float key_ui_left = KEY_FARLEFT_WIDTH + 10.0f;
	float key_ui_top = KEY_TOP_HEIGHT + 10.0f;

	float expl_ui_left = key_ui_left + KEY_WIDTH * 4.0f;
	float expl_ui_top = key_ui_top + 5.0f;

	if (g_isDisplay) {
		//左上に操作方法表示
		//グレーの枠
		Sprite_Draw(g_WhiteTexId, KEY_FARLEFT_WIDTH, KEY_TOP_HEIGHT, 300.0f, 400.0f, { 0.5f,0.5f,0.5f,0.8f });

		//操作説明表示切替
		Sprite_Draw(g_KeyTABbefore_TexId, key_ui_left - 3.0f, key_ui_top - 7.0f, KEY_WIDTH * 1.5f, KEY_HEIGHT * 1.5f);
		Sprite_Draw(g_Explanation_Swicth_TexId, expl_ui_left, expl_ui_top, EXPLANATION_HEIGHT * 7.0f, EXPLANATION_HEIGHT);

		//左右操作ボタン
		if (!KeyLogger_IsPressed(KK_A)) {
			Sprite_Draw(g_KeyAbefore_TexId, key_ui_left, key_ui_top + KEY_HEIGHT * 1.5f, KEY_WIDTH, KEY_HEIGHT);
		}
		else {
			Sprite_Draw(g_KeyAafter_TexId, key_ui_left, key_ui_top + KEY_HEIGHT * 1.5f, KEY_WIDTH, KEY_HEIGHT);
		}
		if (!KeyLogger_IsPressed(KK_D)) {
			Sprite_Draw(g_KeyDbefore_TexId, key_ui_left + KEY_WIDTH + 5.0f, key_ui_top + KEY_HEIGHT * 1.5f, KEY_WIDTH, KEY_HEIGHT);
		}
		else {
			Sprite_Draw(g_KeyDafter_TexId, key_ui_left + KEY_WIDTH + 5.0f, key_ui_top + KEY_HEIGHT * 1.5f, KEY_WIDTH, KEY_HEIGHT);
		}
		Sprite_Draw(g_Explanation_Move_TexId, expl_ui_left, expl_ui_top + KEY_HEIGHT * 1.5f, EXPLANATION_HEIGHT * 4.0f, EXPLANATION_HEIGHT);

		//ダッシュ
		if (!KeyLogger_IsPressed(KK_LEFTSHIFT)) {
			Sprite_Draw(g_KeySHIFTbefore_TexId, key_ui_left - 3.0f, key_ui_top + KEY_HEIGHT * 1.5f * 2.0f - 7.0f, KEY_WIDTH * 1.5f, KEY_HEIGHT * 1.5f);
		}
		else {
			Sprite_Draw(g_KeySHIFTafter_TexId, key_ui_left - 3.0f, key_ui_top + KEY_HEIGHT * 1.5f * 2.0f - 7.0f, KEY_WIDTH * 1.5f, KEY_HEIGHT * 1.5f);
		}
		Sprite_Draw(g_Explanation_Dash_TexId, expl_ui_left, expl_ui_top + KEY_HEIGHT * 1.5f * 2.0f, EXPLANATION_HEIGHT * 3.0f, EXPLANATION_HEIGHT);


		//攻撃ボタン
		if (!KeyLogger_IsPressed(KK_J)) {
			Sprite_Draw(g_KeyJbefore_TexId, key_ui_left, key_ui_top + KEY_HEIGHT * 1.5f * 3.0f, KEY_WIDTH, KEY_HEIGHT);
		}
		else {
			Sprite_Draw(g_KeyJafter_TexId, key_ui_left, key_ui_top + KEY_HEIGHT * 1.5f * 3.0f, KEY_WIDTH, KEY_HEIGHT);
		}
		Sprite_Draw(g_Explanation_Attack_TexId, expl_ui_left, expl_ui_top + KEY_HEIGHT * 1.5f * 3.0f, EXPLANATION_HEIGHT * 7.0f, EXPLANATION_HEIGHT);

		//1秒攻撃ボタン長押し
		Sprite_Draw(g_KeyJafter_TexId, key_ui_left, key_ui_top + KEY_HEIGHT * 1.5f * 4.0f, KEY_WIDTH, KEY_HEIGHT);
		Sprite_Draw(g_HoldTexId, key_ui_left + KEY_WIDTH + 5.0f, key_ui_top + KEY_HEIGHT * 1.5f * 4.0f, KEY_WIDTH * 2.0f, KEY_HEIGHT);
		Sprite_Draw(g_Explanation_StrongAttack_TexId, expl_ui_left, expl_ui_top + KEY_HEIGHT * 1.5f * 4.0f, EXPLANATION_HEIGHT * 8.0f, EXPLANATION_HEIGHT);

		//攻撃ボタン離し
		Sprite_Draw(g_KeyJbefore_TexId, key_ui_left, key_ui_top + KEY_HEIGHT * 1.5f * 5.0f, KEY_WIDTH, KEY_HEIGHT);
		Sprite_Draw(g_ReleaseTexId, key_ui_left + KEY_WIDTH + 5.0f, key_ui_top + KEY_HEIGHT * 1.5f * 5.0f, KEY_WIDTH * 3.0f, KEY_HEIGHT);
		Sprite_Draw(g_Explanation_Bullet_TexId, expl_ui_left, expl_ui_top + KEY_HEIGHT * 1.5f * 5.0f - EXPLANATION_HEIGHT / 2.0f, EXPLANATION_HEIGHT * 7.0f, EXPLANATION_HEIGHT * 2.0f);


		//ジャンプボタン
		if (!KeyLogger_IsPressed(KK_SPACE)) {
			Sprite_Draw(g_KeySPACEbefore_TexId, key_ui_left - 3.0f, key_ui_top + KEY_HEIGHT * 1.5f * 6.0f - 7.0f, KEY_WIDTH * 1.5f, KEY_HEIGHT * 1.5f);
		}
		else {
			Sprite_Draw(g_KeySPACEafter_TexId, key_ui_left - 3.0f, key_ui_top + KEY_HEIGHT * 1.5f * 6.0f - 7.0f, KEY_WIDTH * 1.5f, KEY_HEIGHT * 1.5f);
		}
		Sprite_Draw(g_Explanation_Jump_TexId, expl_ui_left, expl_ui_top + KEY_HEIGHT * 1.5f * 6.0f, EXPLANATION_HEIGHT * 4.0f, EXPLANATION_HEIGHT);


		//足場を降りるボタン
		if (!KeyLogger_IsPressed(KK_S)) {
			Sprite_Draw(g_KeySbefore_TexId, key_ui_left, key_ui_top + KEY_HEIGHT * 1.5f * 7.0f, KEY_WIDTH, KEY_HEIGHT);
		}
		else {
			Sprite_Draw(g_KeySafter_TexId, key_ui_left, key_ui_top + KEY_HEIGHT * 1.5f * 7.0f, KEY_WIDTH, KEY_HEIGHT);
		}
		Sprite_Draw(g_KeyPLUSbefore_TexId, key_ui_left + KEY_WIDTH + 5.0f, key_ui_top + KEY_HEIGHT * 1.5f * 7.0f, KEY_WIDTH, KEY_HEIGHT);
		if (!KeyLogger_IsPressed(KK_SPACE)) {
			Sprite_Draw(g_KeySPACEbefore_TexId, key_ui_left + KEY_WIDTH * 2.0f + 5.0f - 3.0f, key_ui_top + KEY_HEIGHT * 1.5f * 7.0f - 7.0f, KEY_WIDTH * 1.5f, KEY_HEIGHT * 1.5f);
		}
		else {
			Sprite_Draw(g_KeySPACEafter_TexId, key_ui_left + KEY_WIDTH * 2.0f + 5.0f - 3.0f, key_ui_top + KEY_HEIGHT * 1.5f * 7.0f - 7.0f, KEY_WIDTH * 1.5f, KEY_HEIGHT * 1.5f);
		}
		Sprite_Draw(g_Explanation_Getoff_TexId, expl_ui_left, expl_ui_top + KEY_HEIGHT * 1.5f * 7.0f, EXPLANATION_HEIGHT * 8.0f, EXPLANATION_HEIGHT);

		//ポーズ
		Sprite_Draw(g_KeyESCAPEbefore_TexId, key_ui_left, key_ui_top + KEY_HEIGHT * 1.5f * 8.0f, KEY_WIDTH, KEY_HEIGHT);
		Sprite_Draw(g_Explanation_Pause_TexId, expl_ui_left, expl_ui_top + KEY_HEIGHT * 1.5f * 8.0f, EXPLANATION_HEIGHT * 3.0f, EXPLANATION_HEIGHT);
	}
	else {
		//グレーの枠
		Sprite_Draw(g_WhiteTexId, KEY_FARLEFT_WIDTH, KEY_TOP_HEIGHT, 300.0f, 50.0f, { 0.5f,0.5f,0.5f,0.8f });

		//操作説明表示切替
		Sprite_Draw(g_KeyTABbefore_TexId, key_ui_left - 3.0f, key_ui_top - 7.0f, KEY_WIDTH * 1.5f, KEY_HEIGHT * 1.5f);
		Sprite_Draw(g_Explanation_Swicth_TexId, expl_ui_left, expl_ui_top, EXPLANATION_HEIGHT * 7.0f, EXPLANATION_HEIGHT);
	}


	// 右上にHPバーの枠を表示
	Sprite_Draw(g_HpFrameTexId, screen_width - 680.0f, 10.0f, 680.0f, 160.0f);

	float damage_bar_ratio = g_HpRedValue / Player_MAXHP;
	Sprite_Draw(g_HpRedTexId, screen_width - 550.0f, 20.0f, 500.0f * damage_bar_ratio, 60.0f);

	float redMp_bar_ratio = g_MpRedValue / Player_MAXMP;
	Sprite_Draw(g_HpRedTexId, screen_width - 550.0f, 90.0f, 500.0f * redMp_bar_ratio, 60.0f);

	// 現在のHPの割合を計算 (0.0～1.0)
	float hp_ratio = Player_GetHp() / Player_MAXHP;
	// HPの割合に応じて、中身のバーの描画する幅を変える
	Sprite_Draw(g_HpBarTexId, screen_width - 550.0f, 20.0f, 500.0f * hp_ratio, 60.0f);

	float mp_ratio = Player_GetMp() / Player_MAXMP;
	Sprite_Draw(g_MpBarTexId, screen_width - 550.0f, 90.0f, 500.0f * mp_ratio, 60.0f);

	//HPの文字
	Sprite_Draw(g_HpCharTexId, screen_width - 660.0f, 20.0f, 120.0f, 60.0f);
	//MPの文字
	Sprite_Draw(g_MpCharTexId, screen_width - 660.0f, 90.0f, 120.0f, 60.0f);
}

bool isHpDecrease() {
	return g_isHpDecrease;
}

bool isMpDecrease() {
	return g_isMpDecrease;
}

void PlayerUI_Switch() {
	g_isDisplay = !g_isDisplay;
}
