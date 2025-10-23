/*==============================================================================

   マップ移動キャラクター制御 [player.cpp]
														 Author : Harada Ren
														 Date   : 2025/07/17
--------------------------------------------------------------------------------

==============================================================================*/
#include "player.h"
#include "DirectXMath.h"
using namespace DirectX;
#include "texture.h"
#include "sprite.h"
#include "sprite_anim.h"
#include "key_logger.h"
#include "bullet.h"
#include "collision.h"
#include "direct3d.h"
#include "map.h"
#include "Audio.h"
#include "audio_manager.h"
#include "enemy.h"
#include "mouse.h"
#include "player_ui.h"
#include "particle.h"

static int g_PlayerTexId = -1;
static int AnimId_Player = -1;
static int PlayId_Player = -1;

static int g_AttackTexId = -1;
static int AnimId_Attack = -1;
static int PlayId_Attack = -1;

static int g_AttackEndTexId = -1;
static int AnimId_AttackEnd = -1;
static int PlayId_AttackEnd = -1;

static int g_StrongAttackEndTexId = -1;
static int AnimId_StrongAttackEnd = -1;
static int PlayId_StrongAttackEnd = -1;


static XMFLOAT2 g_PlayerPosition{}; //world座標
static XMFLOAT2 g_PlayerVelocity{}; //速度

static XMFLOAT2 g_PrevPlayerPosition{}; //前のフレームの位置を記憶する

static Circle g_PlayerCollision{ {60.0f,60.0f},40.0f };
static bool g_PlayerEnable = true;

//右を向いていたらtrue 左だったらfalse
static bool g_IsPlayerFacingRight = true;

static int g_GunSoundId = -1;

static bool g_IsJump = false;

static PLAYER_STATE g_PlayerState = STATE_STOP;

static double g_OnewayIgnoreTimer = 0.0;

static double g_coyoteTimer = 0.0;

static double g_AttackHoldTime = 0.0;

static int g_BladeAudioId = -1;
static int g_ChargeAudioId = -1;
static int g_DamageSEId = -1;

static float g_PlayerHp = Player_MAXHP;
static float g_PlayerInvincibleTime = 0.0f;

static float g_PlayerMp = Player_MAXMP;

static double g_StrongAttackTimer = 0.0;

static int g_StrongAttackShotCount = 0;
static double g_StrongAttackShotIntervalTimer = 0.0;

static constexpr double SHOT_INTERVAL = 0.15;

static OBB g_AttackCollisions[6];

void Player_Initialize(const XMFLOAT2& position) {
	g_PlayerTexId = Texture_Load(L"resource/texture/player/hal_chang_anim.png");
	g_AttackTexId = Texture_Load(L"resource/texture/player/hal_chang_attack_anim.png");
	g_AttackEndTexId = Texture_Load(L"resource/texture/player/hal_chang_attack_end_anim.png");
	g_StrongAttackEndTexId = Texture_Load(L"resource/texture/player/hal_chang_strongattack_end_anim.png");

	AnimId_Player = SpriteAnim_RegisterPattern(g_PlayerTexId, 16, 8, 0.07, { 1024,1024 }, { 0,0 });
	PlayId_Player = SpriteAnim_CreatePlayer(AnimId_Player);

	AnimId_Attack = SpriteAnim_RegisterPattern(g_AttackTexId, 4, 4, 0.03, { 2048,2048 }, { 0,0 }, false);
	PlayId_Attack = SpriteAnim_CreatePlayer(AnimId_Attack);

	AnimId_AttackEnd = SpriteAnim_RegisterPattern(g_AttackEndTexId, 7, 7, 0.03, { 2048,2048 }, { 0,0 }, false);
	PlayId_AttackEnd = SpriteAnim_CreatePlayer(AnimId_AttackEnd);

	AnimId_StrongAttackEnd = SpriteAnim_RegisterPattern(g_StrongAttackEndTexId, 9, 5, 0.05, { 2048,2048 }, { 0,0 }, false);
	PlayId_StrongAttackEnd = SpriteAnim_CreatePlayer(AnimId_StrongAttackEnd);

	g_PlayerPosition = position;
	g_PlayerVelocity = { 0.0f,0.0f };
	g_PlayerEnable = true;

	g_PlayerState = STATE_STOP;

	g_IsPlayerFacingRight = true;

	g_BladeAudioId = LoadAudio("resource/audio/blade1.wav");
	g_ChargeAudioId = LoadAudio("resource/audio/charge.wav");
	g_DamageSEId = LoadAudio("resource/audio/damage.wav");

	g_PlayerHp = Player_MAXHP;
	g_PlayerMp = Player_MAXMP;
	g_PlayerInvincibleTime = 0.0f;

	g_OnewayIgnoreTimer = 0.0;
	g_coyoteTimer = 0.0;

	g_StrongAttackTimer = 0.0;

	g_StrongAttackShotCount = 0;
	g_StrongAttackShotIntervalTimer = 0.0;

	g_AttackCollisions[0] = { {0.0f, 0.0f}, {0.0f, 0.0f}, {{1,0}, {0,1}} };
	g_AttackCollisions[1] = { {0.0f, -90.0f}, {70.0f, 30.0f}, {{1,0}, {0,1}} };
	g_AttackCollisions[2] = { {30.0f, -70.0f}, {70.0f, 30.0f}, {{1,0}, {0,1}} };
	g_AttackCollisions[3] = { {60.0f, -50.0f}, {70.0f, 30.0f}, {{1,0}, {0,1}} };
	g_AttackCollisions[4] = { {80.0f, -10.0f}, {70.0f, 30.0f}, {{1,0}, {0,1}} };
	g_AttackCollisions[5] = { {100.0f, 40.0f}, {70.0f, 30.0f}, {{1,0}, {0,1}} };
}

void Runnner_Finalize() {
}

void Player_Update(double elapsed_time) {
	if (!g_PlayerEnable) return;

	//-----時間経過処理-----
	if (g_PlayerInvincibleTime > 0.0f) {
		g_PlayerInvincibleTime -= (float)elapsed_time;
	}

	if (g_coyoteTimer > 0.0) {
		g_coyoteTimer -= elapsed_time;
	}

	//赤ゲージがなければHP,MPを回復
	if (g_PlayerHp<Player_MAXHP && !isHpDecrease()) {
		g_PlayerHp += 0.5f * (float)elapsed_time;
		if (g_PlayerHp >= Player_MAXHP) {
			g_PlayerHp = Player_MAXHP;
		}
	}
	if (g_PlayerMp<Player_MAXMP && !isMpDecrease()) {
		g_PlayerMp += 1.0f * (float)elapsed_time;
		if (g_PlayerMp >= Player_MAXMP) {
			g_PlayerMp = Player_MAXMP;
		}
	}


	//-----左右移動-----
	//前の位置を保存
	g_PrevPlayerPosition = g_PlayerPosition;

	// 演算できる構造体に変換
	XMVECTOR position = XMLoadFloat2(&g_PlayerPosition);
	XMVECTOR velocity = XMLoadFloat2(&g_PlayerVelocity);

	// 地上での左右移動を速度で制御
	if (!g_IsJump) {
		float move_speed = 500.0f; // 地上での移動スピード
		float dash_speed = 800.0f; //ダッシュ時の移動スピード
		if (g_PlayerState == STATE_RUN) {
			move_speed = dash_speed;
		}
		if (KeyLogger_IsPressed(KK_D)) {
			velocity = XMVectorSetX(velocity, move_speed);
			if (g_PlayerState != STATE_ATTACKEND && g_PlayerState != STATE_STRONGATTACKEND) {
				g_IsPlayerFacingRight = true;
			}
		}
		else if (KeyLogger_IsPressed(KK_A)) {
			velocity = XMVectorSetX(velocity, -move_speed);
			if (g_PlayerState != STATE_ATTACKEND && g_PlayerState != STATE_STRONGATTACKEND) {
				g_IsPlayerFacingRight = false;
			}
		}
		else {
			// キーが押されてなければ横方向の速度を0にする
			velocity = XMVectorSetX(velocity, 0.0f);
		}
	}
	else{
		// 空中での移動処理
		float aerial_control_force = 1500.0f; // 空中で左右に加える力の大きさ
		float max_aerial_speed = 500.0f;      // 空中での横方向の最高速度
		if (g_PlayerState == STATE_RUN) {
			max_aerial_speed = 800.0f;
		}
		// キー入力に応じて、左右に力を加える（加速度）
		if (KeyLogger_IsPressed(KK_D)) {
			velocity += XMVectorSet(aerial_control_force * (float)elapsed_time, 0.0f, 0.0f, 0.0f);
			if (g_PlayerState != STATE_ATTACKEND && g_PlayerState != STATE_STRONGATTACKEND) {
				g_IsPlayerFacingRight = true;
			}
		}
		if (KeyLogger_IsPressed(KK_A)) {
			velocity -= XMVectorSet(aerial_control_force * (float)elapsed_time, 0.0f, 0.0f, 0.0f);
			if (g_PlayerState != STATE_ATTACKEND && g_PlayerState != STATE_STRONGATTACKEND) {
				g_IsPlayerFacingRight = false;
			}
		}

		// 空中での横速度が最高速度を超えないように調整する
		float vx = XMVectorGetX(velocity);
		if (vx > max_aerial_speed) {
			velocity = XMVectorSetX(velocity, max_aerial_speed);
		}
		if (vx < -max_aerial_speed) {
			velocity = XMVectorSetX(velocity, -max_aerial_speed);
		}
	}

	// ワンウェイ床から降りる処理
	if (g_OnewayIgnoreTimer > 0.0) {
		g_OnewayIgnoreTimer -= elapsed_time;
	}
	if (g_coyoteTimer > 0.0 && KeyLogger_IsPressed(KK_S) && KeyLogger_IsTrigger(KK_SPACE)) {
		g_IsJump = true;
		//床無視タイマーをセット
		g_OnewayIgnoreTimer = 0.2;
		g_coyoteTimer = 0.0;
	}



	// -----ジャンプ-----
	// コヨーテタイム以内だったらジャンプ可能
	if (KeyLogger_IsTrigger(KK_SPACE) && g_coyoteTimer>0.0) {
		float jump_power = -1000.0f; // Y方向のジャンプ力
		// Y方向の速度をジャンプ力で上書き
		velocity = XMVectorSetY(velocity, jump_power);
		g_IsJump = true;
		g_coyoteTimer = 0.0;
	}

	// 重力
	XMFLOAT2 gdir{ 0.0f, 1.0f };
	float gravity = 9.8f * 200.0f * (float)elapsed_time;
	velocity += XMLoadFloat2(&gdir) * gravity;



	// -----当たり判定-----
	// 横方向に移動して壁との当たり判定
	XMVECTOR horizontal_move = XMVectorSet(XMVectorGetX(velocity) * (float)elapsed_time, 0.0f, 0.0f, 0.0f);
	position += horizontal_move;
	XMStoreFloat2(&g_PlayerPosition, position);

	if (Map_hitJudgementBoxVSMap(Player_GetBoxCollision())) {
		position -= horizontal_move; // めり込んだ分を戻す
		velocity = XMVectorSetX(velocity, 0.0f); // 横方向の速度を0に
		XMStoreFloat2(&g_PlayerPosition, position);
	}
	else {
		if (XMVectorGetY(velocity) > 0.0f) {
			g_IsJump = true;
		}
	}

	// 縦方向に移動して地面/天井との当たり判定
	XMVECTOR vertical_move = XMVectorSet(0.0f, XMVectorGetY(velocity) * (float)elapsed_time, 0.0f, 0.0f);
	position += vertical_move;
	XMStoreFloat2(&g_PlayerPosition, position);

	Box player_box = Player_GetBoxCollision();

	if (Map_hitJudgementBoxVSMap(player_box)) {
		position -= vertical_move; // めり込んだ分を戻す

		// 落下中に地面にぶつかったら着地とみなす
		if (XMVectorGetY(velocity) > 0) {
			g_IsJump = false;
			g_coyoteTimer = 0.1;
		}

		velocity = XMVectorSetY(velocity, 0.0f); // 天井か地面にぶつかったら縦方向の速度を0に
		XMStoreFloat2(&g_PlayerPosition, position);
	}
	else if (g_OnewayIgnoreTimer <= 0.0) {
		// 判定用の一時変数
		int player_map_y = Map_GetWorldToMapY(player_box.center.y + player_box.half_height);
		float platform_top_y = (float)player_map_y * MAPCHIP_HEIGHT;
		float player_bottom_y = player_box.center.y + player_box.half_height;

		// プレイヤーの足元にワンウェイ床判定用Boxを作成
		Box foot_box = player_box;
		foot_box.center.y = player_bottom_y;
		foot_box.half_height = 5.0f;
		foot_box.half_width = 10.0f;

		if (Map_hitJudgementBoxVSOneway(foot_box)) {
			// 落下中かつ、前フレームで足が床より上にあったら着地
			if (XMVectorGetY(velocity) >= 0 && (g_PrevPlayerPosition.y + player_box.half_height) <= platform_top_y) {
				position -= vertical_move; // めり込んだ分を戻す
				g_IsJump = false;
				velocity = XMVectorSetY(velocity, 0.0f);
				g_coyoteTimer = 0.1;
			}
		}
	}

	// 最終的な速度と位置を保存
	XMStoreFloat2(&g_PlayerVelocity, velocity);
	XMStoreFloat2(&g_PlayerPosition, position);



	//-----プレイヤーのステート管理-----
	// 攻撃中でなければ攻撃を開始できる
	if (KeyLogger_IsTrigger(KK_J)) {
		if (g_PlayerState < STATE_ATTACK) {
			g_PlayerState = STATE_ATTACK;
			SpriteAnim_DestroyPlayer(PlayId_Attack);
			PlayId_Attack = SpriteAnim_CreatePlayer(AnimId_Attack);
		}
	}

	//攻撃ボタンを押している間タイマーを進める
	if (g_PlayerState == STATE_ATTACK) {
		if (KeyLogger_IsPressed(KK_J)) {
			g_PlayerState = STATE_ATTACK;
			g_AttackHoldTime += elapsed_time;
			
			//0.9秒ホールドしたら強攻撃に移行
			if (g_AttackHoldTime > 0.9) {
				g_PlayerState = STATE_STRONGATTACK;
				PlayAudio(g_ChargeAudioId);
			}
		}
	}

	//チャージ時パーティクル生成
	if (g_PlayerState == STATE_STRONGATTACK) {
		Particle_Create(ParticleType::CHARGE, { g_PlayerPosition.x + Player_WIDTH / 2.0f, g_PlayerPosition.y + Player_HEIGHT / 2.0f });
	}

	//攻撃ボタンが話された瞬間にどの攻撃を出すか判定
	if (KeyLogger_IsReleased(KK_J)) {
		//0.9秒押されていたら強攻撃
		if (g_PlayerState == STATE_STRONGATTACK) {
			g_PlayerState = STATE_STRONGATTACKEND;

			g_StrongAttackShotCount = 0;
			g_StrongAttackShotIntervalTimer = 0.0;

			//カメラを揺らす
			Map_StartShake(10.0f, 0.5);

			// 攻撃アニメを再生する直前にリセット
			SpriteAnim_DestroyPlayer(PlayId_StrongAttackEnd);
			PlayId_StrongAttackEnd = SpriteAnim_CreatePlayer(AnimId_StrongAttackEnd);
			PlayAudio(g_BladeAudioId);
		}
		//単押し(0.9秒以内)であれば通常攻撃
		else if (g_PlayerState == STATE_ATTACK) {
			g_PlayerState = STATE_ATTACKEND;
			SpriteAnim_DestroyPlayer(PlayId_AttackEnd);
			PlayId_AttackEnd = SpriteAnim_CreatePlayer(AnimId_AttackEnd);
			PlayAudio(g_BladeAudioId);
		}
		g_AttackHoldTime = 0.0;
	}


	//攻撃中・攻撃終了中
	if (g_PlayerState == STATE_ATTACKEND) {
		if (SpriteAnim_IsStopped(PlayId_AttackEnd)) {
			g_PlayerState = STATE_STOP;

			if (g_PlayerMp >= 1.0f) {
				//弾を1発発射
				XMFLOAT2 bullet_pos;
				if (g_IsPlayerFacingRight) {
					// 右向きの時はプレイヤーの右側に弾を出す
					bullet_pos = { g_PlayerPosition.x + 80.0f, g_PlayerPosition.y - 50.0f };
				}
				else {
					// 左向きの時はプレイヤーの左側に弾を出す
					bullet_pos = { g_PlayerPosition.x - 30.0f, g_PlayerPosition.y - 50.0f };
				}

				Bullet_Create({ bullet_pos.x,bullet_pos.y }, Get_NearestTargetPosision(g_PlayerPosition));
				
				g_PlayerMp -= 1.0f;
			}

		}
	}
	else if (g_PlayerState == STATE_STRONGATTACKEND) {
		//強攻撃の経過時間
		g_StrongAttackTimer += elapsed_time;
		//連射用タイマー
		g_StrongAttackShotIntervalTimer += elapsed_time;

		//弾を3発発射
		if (g_PlayerMp >= 0.5f) {
			if (g_StrongAttackShotCount < 3 && g_StrongAttackShotIntervalTimer >= SHOT_INTERVAL) {
				//弾発射
				XMFLOAT2 bullet_pos;
				if (g_IsPlayerFacingRight) {
					bullet_pos = { g_PlayerPosition.x + 60.0f + 30.0f * (float)g_StrongAttackShotCount,
						g_PlayerPosition.y - 70.0f + 30.0f * (float)g_StrongAttackShotCount };
				}
				else {
					bullet_pos = { g_PlayerPosition.x - 10.0f - 30.0f * (float)g_StrongAttackShotCount,
						g_PlayerPosition.y - 70.0f + 30.0f * (float)g_StrongAttackShotCount };
				}
				Bullet_Create(bullet_pos, Get_NearestTargetPosision(g_PlayerPosition));
				g_PlayerMp -= 0.5f;

				g_StrongAttackShotCount++;
				g_StrongAttackShotIntervalTimer = 0.0;
			}
		}

		if (SpriteAnim_IsStopped(PlayId_StrongAttackEnd)) {
			g_PlayerState = STATE_STOP;
		}
	}
	else {
		//状態が変わったらリセット
		g_StrongAttackTimer = 0.0;
	}

	// 攻撃中でなければ、移動やダッシュの状態をチェック	
	if (!g_IsJump && g_PlayerState < STATE_ATTACK) {
		if (KeyLogger_IsPressed(KK_LEFTSHIFT)) {
			g_PlayerState = STATE_RUN;
		}
		else if (KeyLogger_IsPressed(KK_D) || KeyLogger_IsPressed(KK_A)) {
			g_PlayerState = STATE_WALK;
		}
		else {
			g_PlayerState = STATE_STOP;
		}
	}
}


void Player_Draw() {
	if (!g_PlayerEnable) return;

	// カメラのオフセットを取得
	XMFLOAT2 offset = Map_GetWorldOffset();

	// プレイヤーのワールド座標からスクリーン座標を計算
	float screen_x = g_PlayerPosition.x - offset.x;
	float screen_y = g_PlayerPosition.y - offset.y;

	//無敵時間だったら点滅させる
	if (g_PlayerInvincibleTime > 0.0f) {
		if ((int)(g_PlayerInvincibleTime * 10.0f) % 2 == 0) {
			return;
		}
	}

	switch (g_PlayerState) {
	case STATE_STOP:
		Sprite_Draw(g_PlayerTexId, screen_x, screen_y, 128.0f, 128.0f, 0, 0, 1024, 1024, !g_IsPlayerFacingRight);
		break;

	case STATE_WALK:
		SpriteAnim_Draw(PlayId_Player, screen_x, screen_y, Player_WIDTH, Player_HEIGHT, !g_IsPlayerFacingRight);
		break;

	case STATE_RUN:
		SpriteAnim_Draw(PlayId_Player, screen_x, screen_y, Player_WIDTH, Player_HEIGHT, !g_IsPlayerFacingRight);
		break;

	case STATE_ATTACK:
		if (g_IsPlayerFacingRight) {
			SpriteAnim_Draw(PlayId_Attack, screen_x, screen_y - 128.0f, 256.0f, 256.0f);
		}
		else {
			SpriteAnim_Draw(PlayId_Attack, screen_x - 128.0f, screen_y - 128.0f, 256.0f, 256.0f, true);
		}
		break;

	case STATE_ATTACKEND:
		if (g_IsPlayerFacingRight) {
			SpriteAnim_Draw(PlayId_AttackEnd, screen_x, screen_y - 128.0f, 256.0f, 256.0f);
		}
		else {
			SpriteAnim_Draw(PlayId_AttackEnd, screen_x - 128.0f, screen_y - 128.0f, 256.0f, 256.0f, true);
		}
		break;

	case STATE_STRONGATTACK:
		if (g_IsPlayerFacingRight) {
			Sprite_Draw(g_StrongAttackEndTexId, screen_x, screen_y - 384.0f, 512.0f, 512.0f, 0, 0, 2048, 2048);
		}
		else {
			Sprite_Draw(g_StrongAttackEndTexId, screen_x - 384.0f, screen_y - 384.0f, 512.0f, 512.0f, 0, 0, 2048, 2048, true);
		}

		break;

	case STATE_STRONGATTACKEND:
		if (g_IsPlayerFacingRight) {
			SpriteAnim_Draw(PlayId_StrongAttackEnd, screen_x, screen_y - 384.0f, 512.0f, 512.0f);
		}
		else {
			SpriteAnim_Draw(PlayId_StrongAttackEnd, screen_x - 384.0f, screen_y - 384.0f, 512.0f, 512.0f, true);
		}
		break;

	default:
		break;
	}
}

bool Player_IsEnable(){
	return g_PlayerEnable;
}

Circle Player_GetCollision(){
	float cx = g_PlayerPosition.x + (Player_WIDTH * 0.5f);
	float cy = g_PlayerPosition.y + (Player_HEIGHT * 0.5f);
	return { {cx,cy},g_PlayerCollision.radius };
}

Box Player_GetBoxCollision(){
	//中心座標
	float hw = Player_WIDTH * 0.5f;
	float hh = Player_HEIGHT * 0.5f;
	return { {g_PlayerPosition.x + hw,g_PlayerPosition.y + hh + 10},hw - 30,hh - 10 };
}

OBB Player_GetAttackCollision(){	
	// 攻撃中以外は当たり判定なし
	if (g_PlayerState != STATE_ATTACKEND && g_PlayerState != STATE_STRONGATTACKEND) {
		return { {0,0}, {0,0}, {{1,0},{0,1}} };
	}

	int current_frame = -1;
	if (g_PlayerState == STATE_ATTACKEND) {
		current_frame = SpriteAnim_GetPatternNum(PlayId_AttackEnd);
	}
	else {
		// 強攻撃用の当たり判定とアニメーションIDを使う
		current_frame = SpriteAnim_GetPatternNum(PlayId_StrongAttackEnd);
	}
	
	OBB local_obb = g_AttackCollisions[current_frame];
	if (local_obb.half_extent.x == 0) return local_obb; // 当たり判定なしフレーム

	//強攻撃だったら拡大する
	if (g_PlayerState == STATE_STRONGATTACKEND) {
		OBB strong_local_obb = local_obb;
		strong_local_obb.half_extent.x *= 3.0f;
		strong_local_obb.half_extent.y *= 1.5f;

		//拡大でずれた分をずらす　　
		strong_local_obb.center.x += (strong_local_obb.half_extent.x - local_obb.half_extent.x) / 2.0f;
		strong_local_obb.center.y -= (strong_local_obb.half_extent.y - local_obb.half_extent.y) / 2.0f;

		local_obb = strong_local_obb;
	}

	// 剣の角度をフレームに合わせて変える
	float angle = XM_PI;
	switch (current_frame) {
	case 1:
		angle /= -2.0f; // 約90度
		break;

	case 2:
		angle /= -3.0f; //約60度
		break;

	case 3:
		angle /= -4.0f; //約45度
		break;

	case 4:
		angle /= -6.0f; //約30度
		break;

	case 5:
		angle = 0.0f; //0度
		break;

	default:
		break;
	}

	// プレイヤーが左向きなら、ローカル座標と角度を反転
	if (!g_IsPlayerFacingRight) {
		local_obb.center.x *= -1.0f;
		angle *= -1.0f;
	}

	// 角度からOBBの軸ベクトルを計算
	float c = cosf(angle);
	float s = sinf(angle);
	local_obb.axis[0] = { c, s };
	local_obb.axis[1] = { -s, c };

	// ワールド座標に変換
	local_obb.center.x += g_PlayerPosition.x + (Player_WIDTH * 0.5f);
	local_obb.center.y += g_PlayerPosition.y + (Player_HEIGHT * 0.5f);

	return local_obb;
}

void Player_Destroy() {
	g_PlayerEnable = false;
}

DirectX::XMFLOAT2 Player_GetPosition() {
	return g_PlayerPosition;
}


void Player_Damage(float damage) {
	if (g_PlayerInvincibleTime > 0.0f) {
		return;
	}

	g_PlayerHp -= damage;
	if (g_PlayerHp < 0) {
		g_PlayerHp = 0;
	}

	PlayAudio(g_DamageSEId);
	g_PlayerInvincibleTime = 1.0f;
}

float Player_GetHp() {
	return g_PlayerHp;
}

float Player_GetMp(){
	return g_PlayerMp;
}

bool Player_IsDead() {
	return g_PlayerHp <= 0.0f;
}

PLAYER_STATE Player_GetState(){
	return g_PlayerState;
}

bool Player_IsFacingRight(){
	return g_IsPlayerFacingRight;
}
