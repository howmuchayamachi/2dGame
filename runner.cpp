/*==============================================================================

   マップ移動キャラクター制御 [runner.cpp]
														 Author : Harada Ren
														 Date   : 2025/07/17
--------------------------------------------------------------------------------

==============================================================================*/
#include "runner.h"
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

static int g_RunnerTexId = -1;
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


static XMFLOAT2 g_RunnerPosition{}; //world座標
static XMFLOAT2 g_RunnerVelocity{}; //速度

static XMFLOAT2 g_PrevRunnerPosition{}; //前のフレームの位置を記憶する

static Circle g_RunnerCollision{ {60.0f,60.0f},40.0f };
static bool g_RunnerEnable = true;

//右を向いていたらtrue 左だったらfalse
static bool g_IsRunnerFacingRight = true;

static int g_GunSoundId = -1;

static bool g_IsJump = false;

static PLAYER_STATE g_PlayerState = STATE_STOP;

static double g_OnewayIgnoreTimer = 0.0;

static double g_coyoteTimer = 0.0;

static double g_AttackHoldTime = 0.0;

static int g_BladeAudioId = -1;
static int g_ChargeAudioId = -1;
static int g_DamageSEId = -1;

static float g_RunnerHp = RUNNER_MAXHP;
static float g_RunnerInvincibleTime = 0.0f;

static float g_RunnerMp = RUNNER_MAXMP;

static double g_StrongAttackTimer = 0.0;

static int g_StrongAttackShotCount = 0;
static double g_StrongAttackShotIntervalTimer = 0.0;

static constexpr double SHOT_INTERVAL = 0.15;

static OBB g_AttackCollisions[6];

void Runner_Initialize(const XMFLOAT2& position) {
	g_RunnerTexId = Texture_Load(L"resource/texture/player/hal_chang_anim.png");
	g_AttackTexId = Texture_Load(L"resource/texture/player/hal_chang_attack_anim.png");
	g_AttackEndTexId = Texture_Load(L"resource/texture/player/hal_chang_attack_end_anim.png");
	g_StrongAttackEndTexId = Texture_Load(L"resource/texture/player/hal_chang_strongattack_end_anim.png");

	AnimId_Player = SpriteAnim_RegisterPattern(g_RunnerTexId, 16, 8, 0.07, { 1024,1024 }, { 0,0 });
	PlayId_Player = SpriteAnim_CreatePlayer(AnimId_Player);

	AnimId_Attack = SpriteAnim_RegisterPattern(g_AttackTexId, 4, 4, 0.03, { 2048,2048 }, { 0,0 }, false);
	PlayId_Attack = SpriteAnim_CreatePlayer(AnimId_Attack);

	AnimId_AttackEnd = SpriteAnim_RegisterPattern(g_AttackEndTexId, 7, 7, 0.03, { 2048,2048 }, { 0,0 }, false);
	PlayId_AttackEnd = SpriteAnim_CreatePlayer(AnimId_AttackEnd);

	AnimId_StrongAttackEnd = SpriteAnim_RegisterPattern(g_StrongAttackEndTexId, 9, 5, 0.05, { 2048,2048 }, { 0,0 }, false);
	PlayId_StrongAttackEnd = SpriteAnim_CreatePlayer(AnimId_StrongAttackEnd);

	g_RunnerPosition = position;
	g_RunnerVelocity = { 0.0f,0.0f };
	g_RunnerEnable = true;

	g_PlayerState = STATE_STOP;

	g_IsRunnerFacingRight = true;

	g_BladeAudioId = LoadAudio("resource/audio/blade1.wav");
	g_ChargeAudioId = LoadAudio("resource/audio/charge.wav");
	g_DamageSEId = LoadAudio("resource/audio/damage.wav");

	g_RunnerHp = RUNNER_MAXHP;
	g_RunnerInvincibleTime = 0.0f;

	g_RunnerMp = RUNNER_MAXMP;

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

void Runner_Update(double elapsed_time) {
	if (!g_RunnerEnable) return;

	//時間経過処理
	if (g_RunnerInvincibleTime > 0.0f) {
		g_RunnerInvincibleTime -= (float)elapsed_time;
	}

	if (g_coyoteTimer > 0.0) {
		g_coyoteTimer -= elapsed_time;
	}

	//赤ゲージがなければHP,MPを回復
	if (g_RunnerHp<RUNNER_MAXHP && !isHpDecrease()) {
		g_RunnerHp += 0.5f * (float)elapsed_time;
		if (g_RunnerHp >= RUNNER_MAXHP) {
			g_RunnerHp = RUNNER_MAXHP;
		}
	}
	if (g_RunnerMp<RUNNER_MAXMP && !isMpDecrease()) {
		g_RunnerMp += 1.0f * (float)elapsed_time;
		if (g_RunnerMp >= RUNNER_MAXMP) {
			g_RunnerMp = RUNNER_MAXMP;
		}
	}



	//前の位置を保存
	g_PrevRunnerPosition = g_RunnerPosition;

	// 演算できる構造体に変換
	XMVECTOR position = XMLoadFloat2(&g_RunnerPosition);
	XMVECTOR velocity = XMLoadFloat2(&g_RunnerVelocity);

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
				g_IsRunnerFacingRight = true;
			}
		}
		else if (KeyLogger_IsPressed(KK_A)) {
			velocity = XMVectorSetX(velocity, -move_speed);
			if (g_PlayerState != STATE_ATTACKEND && g_PlayerState != STATE_STRONGATTACKEND) {
				g_IsRunnerFacingRight = false;
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
				g_IsRunnerFacingRight = true;
			}
		}
		if (KeyLogger_IsPressed(KK_A)) {
			velocity -= XMVectorSet(aerial_control_force * (float)elapsed_time, 0.0f, 0.0f, 0.0f);
			if (g_PlayerState != STATE_ATTACKEND && g_PlayerState != STATE_STRONGATTACKEND) {
				g_IsRunnerFacingRight = false;
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



	// ジャンプ
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



	// 当たり判定
	// 横方向に移動して壁との当たり判定
	XMVECTOR horizontal_move = XMVectorSet(XMVectorGetX(velocity) * (float)elapsed_time, 0.0f, 0.0f, 0.0f);
	position += horizontal_move;
	XMStoreFloat2(&g_RunnerPosition, position);

	if (Map_hitJudgementBoxVSMap(Runner_GetBoxCollision())) {
		position -= horizontal_move; // めり込んだ分を戻す
		velocity = XMVectorSetX(velocity, 0.0f); // 横方向の速度を0に
		XMStoreFloat2(&g_RunnerPosition, position);
	}
	else {
		if (XMVectorGetY(velocity) > 0.0f) {
			g_IsJump = true;
		}
	}

	// 縦方向に移動して地面/天井との当たり判定
	XMVECTOR vertical_move = XMVectorSet(0.0f, XMVectorGetY(velocity) * (float)elapsed_time, 0.0f, 0.0f);
	position += vertical_move;
	XMStoreFloat2(&g_RunnerPosition, position);

	Box player_box = Runner_GetBoxCollision();

	if (Map_hitJudgementBoxVSMap(player_box)) {
		position -= vertical_move; // めり込んだ分を戻す

		// 落下中に地面にぶつかったら着地とみなす
		if (XMVectorGetY(velocity) > 0) {
			g_IsJump = false;
			g_coyoteTimer = 0.1;
		}

		velocity = XMVectorSetY(velocity, 0.0f); // 天井か地面にぶつかったら縦方向の速度を0に
		XMStoreFloat2(&g_RunnerPosition, position);
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
			if (XMVectorGetY(velocity) >= 0 && (g_PrevRunnerPosition.y + player_box.half_height) <= platform_top_y) {
				position -= vertical_move; // めり込んだ分を戻す
				g_IsJump = false;
				velocity = XMVectorSetY(velocity, 0.0f);
				g_coyoteTimer = 0.1;
			}
		}
	}

	// 最終的な速度と位置を保存
	XMStoreFloat2(&g_RunnerVelocity, velocity);
	XMStoreFloat2(&g_RunnerPosition, position);




	//プレイヤーのステート管理

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
		Particle_Create(ParticleType::CHARGE, { g_RunnerPosition.x + RUNNER_WIDTH / 2.0f, g_RunnerPosition.y + RUNNER_HEIGHT / 2.0f });
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

			if (g_RunnerMp >= 1.0f) {
				//弾を1発発射
				XMFLOAT2 bullet_pos;
				if (g_IsRunnerFacingRight) {
					// 右向きの時はプレイヤーの右側に弾を出す
					bullet_pos = { g_RunnerPosition.x + 80.0f, g_RunnerPosition.y - 50.0f };
				}
				else {
					// 左向きの時はプレイヤーの左側に弾を出す
					bullet_pos = { g_RunnerPosition.x - 30.0f, g_RunnerPosition.y - 50.0f };
				}

				Bullet_Create({ bullet_pos.x,bullet_pos.y }, Get_NearestTargetPosision(g_RunnerPosition));
				
				g_RunnerMp -= 1.0f;
			}

		}
	}
	else if (g_PlayerState == STATE_STRONGATTACKEND) {
		//強攻撃の経過時間
		g_StrongAttackTimer += elapsed_time;
		//連射用タイマー
		g_StrongAttackShotIntervalTimer += elapsed_time;

		//弾を3発発射
		if (g_RunnerMp >= 0.5f) {
			if (g_StrongAttackShotCount < 3 && g_StrongAttackShotIntervalTimer >= SHOT_INTERVAL) {
				//弾発射
				XMFLOAT2 bullet_pos;
				if (g_IsRunnerFacingRight) {
					bullet_pos = { g_RunnerPosition.x + 60.0f + 30.0f * (float)g_StrongAttackShotCount,
						g_RunnerPosition.y - 70.0f + 30.0f * (float)g_StrongAttackShotCount };
				}
				else {
					bullet_pos = { g_RunnerPosition.x - 10.0f - 30.0f * (float)g_StrongAttackShotCount,
						g_RunnerPosition.y - 70.0f + 30.0f * (float)g_StrongAttackShotCount };
				}
				Bullet_Create(bullet_pos, Get_NearestTargetPosision(g_RunnerPosition));
				g_RunnerMp -= 0.5f;

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


void Runner_Draw() {
	if (!g_RunnerEnable) return;

	// カメラのオフセットを取得
	XMFLOAT2 offset = Map_GetWorldOffset();

	// プレイヤーのワールド座標からスクリーン座標を計算
	float screen_x = g_RunnerPosition.x - offset.x;
	float screen_y = g_RunnerPosition.y - offset.y;

	//無敵時間だったら点滅させる
	if (g_RunnerInvincibleTime > 0.0f) {
		if ((int)(g_RunnerInvincibleTime * 10.0f) % 2 == 0) {
			return;
		}
	}

	switch (g_PlayerState) {
	case STATE_STOP:
		Sprite_Draw(g_RunnerTexId, screen_x, screen_y, 128.0f, 128.0f, 0, 0, 1024, 1024, !g_IsRunnerFacingRight);
		break;

	case STATE_WALK:
		SpriteAnim_Draw(PlayId_Player, screen_x, screen_y, RUNNER_WIDTH, RUNNER_HEIGHT, !g_IsRunnerFacingRight);
		break;

	case STATE_RUN:
		SpriteAnim_Draw(PlayId_Player, screen_x, screen_y, RUNNER_WIDTH, RUNNER_HEIGHT, !g_IsRunnerFacingRight);
		break;

	case STATE_ATTACK:
		if (g_IsRunnerFacingRight) {
			SpriteAnim_Draw(PlayId_Attack, screen_x, screen_y - 128.0f, 256.0f, 256.0f);
		}
		else {
			SpriteAnim_Draw(PlayId_Attack, screen_x - 128.0f, screen_y - 128.0f, 256.0f, 256.0f, true);
		}
		break;

	case STATE_ATTACKEND:
		if (g_IsRunnerFacingRight) {
			SpriteAnim_Draw(PlayId_AttackEnd, screen_x, screen_y - 128.0f, 256.0f, 256.0f);
		}
		else {
			SpriteAnim_Draw(PlayId_AttackEnd, screen_x - 128.0f, screen_y - 128.0f, 256.0f, 256.0f, true);
		}
		break;

	case STATE_STRONGATTACK:
		if (g_IsRunnerFacingRight) {
			Sprite_Draw(g_StrongAttackEndTexId, screen_x, screen_y - 384.0f, 512.0f, 512.0f, 0, 0, 2048, 2048);
		}
		else {
			Sprite_Draw(g_StrongAttackEndTexId, screen_x - 384.0f, screen_y - 384.0f, 512.0f, 512.0f, 0, 0, 2048, 2048, true);
		}

		break;

	case STATE_STRONGATTACKEND:
		if (g_IsRunnerFacingRight) {
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

bool Runner_IsEnable(){
	return g_RunnerEnable;
}

Circle Runner_GetCollision(){
	float cx = g_RunnerPosition.x + (RUNNER_WIDTH * 0.5f);
	float cy = g_RunnerPosition.y + (RUNNER_HEIGHT * 0.5f);
	return { {cx,cy},g_RunnerCollision.radius };
}

Box Runner_GetBoxCollision(){
	//中心座標
	float hw = RUNNER_WIDTH * 0.5f;
	float hh = RUNNER_HEIGHT * 0.5f;
	return { {g_RunnerPosition.x + hw,g_RunnerPosition.y + hh + 10},hw - 30,hh - 10 };
}

OBB Runner_GetAttackCollision(){	
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
	if (!g_IsRunnerFacingRight) {
		local_obb.center.x *= -1.0f;
		angle *= -1.0f;
	}

	// 角度からOBBの軸ベクトルを計算
	float c = cosf(angle);
	float s = sinf(angle);
	local_obb.axis[0] = { c, s };
	local_obb.axis[1] = { -s, c };

	// ワールド座標に変換
	local_obb.center.x += g_RunnerPosition.x + (RUNNER_WIDTH * 0.5f);
	local_obb.center.y += g_RunnerPosition.y + (RUNNER_HEIGHT * 0.5f);

	return local_obb;
}

void Runner_Destroy() {
	g_RunnerEnable = false;
}

DirectX::XMFLOAT2 Runner_GetPosition() {
	return g_RunnerPosition;
}


void Runner_Damage(float damage) {
	if (g_RunnerInvincibleTime > 0.0f) {
		return;
	}

	g_RunnerHp -= damage;
	if (g_RunnerHp < 0) {
		g_RunnerHp = 0;
	}

	PlayAudio(g_DamageSEId);
	g_RunnerInvincibleTime = 1.0f;
}

float Runner_GetHp() {
	return g_RunnerHp;
}

float Runner_GetMp(){
	return g_RunnerMp;
}

bool Runner_IsDead() {
	return g_RunnerHp <= 0.0f;
}

PLAYER_STATE Runner_GetState(){
	return g_PlayerState;
}

bool Runner_IsFacingRight(){
	return g_IsRunnerFacingRight;
}
