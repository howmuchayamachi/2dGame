/*==============================================================================

   ゲーム本体[game.cpp]
														 Author : Harada Ren
														 Date   : 2025/06/27
--------------------------------------------------------------------------------

==============================================================================*/
#include "game.h"
#include "bg.h"
#include "runner.h"
#include "bullet.h"
#include "enemy.h"
#include "enemy_spawner.h"
#include "effect.h"
#include "key_logger.h"
#include "Audio.h"
#include "fade.h"
#include "texture.h"
#include "scene.h"
#include "map.h"
#include "player_ui.h"
#include <DirectXMath.h>
using namespace DirectX;
#include "boss.h"
#include "boss_ui.h"
#include "pause.h"
#include "direct3d.h"
#include "particle.h"
#include "sprite_anim.h"

static int g_BgmId = -1;
static int g_BossBgmId = -1;

static int g_BulletHitSEId = -1;
static int g_SwordHitSEId = -1;
static int g_MenuSEId = -1;


static bool g_GameStart = false;
static bool g_GameEnd = false;

static constexpr int SPANER_NUM = 128;
static bool g_IsSpawned[SPANER_NUM]{};

static bool g_IsBossBattle = false;

static bool g_IsCheckpoint = false;

//前回死んだ位置を保存(初期位置は左上)
static XMFLOAT2 g_PrevRunner_position = { 10.0f * MAPCHIP_WIDTH,15.0f * MAPCHIP_HEIGHT };

void Game_Initialize(){
	Fade_Start(1.0, false);

	g_IsBossBattle = false;

	g_BgmId = LoadAudio("resource/audio/bgm/mainBGM.wav");
	g_BossBgmId= LoadAudio("resource/audio/bgm/bossBGM.wav");
	
	g_BulletHitSEId = LoadAudio("resource/audio/bullet_hit.wav");
	g_SwordHitSEId = LoadAudio("resource/audio/sword_hit.wav");
	g_MenuSEId = LoadAudio("resource/audio/menu.wav");

	BG_Initialize();
	Map_Initialize();
	SpriteAnim_Initialize();
	Bullet_Initialize();
	EnemySpawner_Initialize();
	Effect_Initialize();
	Particle_Initialize();
	Enemy_Initialize();
	Boss_Initialize();

	//コンティニューだったら前回死んだ位置で初期化
	if (!g_IsCheckpoint) {
		Runner_Initialize(g_PrevRunner_position);
	}
	//ボス戦からだったらボス戦直前からスタート
	else {
		Runner_Initialize({ 260.0f * MAPCHIP_WIDTH,15.0f * MAPCHIP_HEIGHT });
	}
	//Runner_Initialize({ 260.0f * MAPCHIP_WIDTH,25.0f * MAPCHIP_HEIGHT });

	PlayerUI_Initialize();
	BossUI_Initialize();

	for (bool& is : g_IsSpawned) {
		is = false;
	}

	PlayAudio(g_BgmId, true); //loadが終わりきる前にplayするとエラー

	g_GameStart = false;
	g_GameEnd = false;

	
}

void Game_Finalize(){
	StopAllAudio();
	UnloadAllAudio();

	SpriteAnim_Finalize();

	BossUI_Finalize();
	Boss_Finalize();
	PlayerUI_Finalize();
	Particle_Finalize();
	Effect_Finalize();
	EnemySpawner_Finalize();
	Enemy_Finalize();
	Bullet_Finalize();
	Runnner_Finalize();
	Map_Finalize();
	BG_Finalize();
}

void Game_Update(double elapsed_time){
	if (!g_GameStart && Fade_GetState() == FADE_STATE_FINISHED_IN) {
		g_GameStart = true;
	}

	if (g_GameStart) {
		if (KeyLogger_IsTrigger(KK_ESCAPE)) {
			Game_SetPause(true);
			PlayAudio(g_MenuSEId);
			Scene_Push(SCENE_PAUSE);
			return;
		}

		if (g_IsBossBattle) {
			Boss_Update(elapsed_time);
			BossUI_Update(elapsed_time);
		}

		BG_Update();
		Runner_Update(elapsed_time);
		Bullet_Update(elapsed_time);
		Map_UpdateCamera(Runner_GetPosition() , elapsed_time);
		
		EnemySpawner_Update(elapsed_time);
		Enemy_Update(elapsed_time);


		if (!g_IsSpawned[0] && CheckEnemyTriggerArea(61 * MAPCHIP_WIDTH, 86 * MAPCHIP_WIDTH, 25 * MAPCHIP_HEIGHT, 33 * MAPCHIP_HEIGHT)) {
			EnemySpawner_Create({ 35.0f * MAPCHIP_WIDTH,45.0f * MAPCHIP_HEIGHT }, ENEMY_TYPE_SLIME_GREEN, 0.0, 1.0, 2, true);
			g_IsSpawned[0] = true;
		}
		if (!g_IsSpawned[1] && CheckEnemyTriggerArea(0 * MAPCHIP_WIDTH, 32 * MAPCHIP_WIDTH, 63 * MAPCHIP_HEIGHT, 65 * MAPCHIP_HEIGHT)) {
			EnemySpawner_Create({ 50.0f * MAPCHIP_WIDTH,85.0f * MAPCHIP_HEIGHT }, ENEMY_TYPE_SLIME_ORANGE, 0.0, 0.5, 3);
			g_IsSpawned[1] = true;
		}
		if (!g_IsSpawned[2] && CheckEnemyTriggerArea(86 * MAPCHIP_WIDTH,  90* MAPCHIP_WIDTH, 65 * MAPCHIP_HEIGHT, 91 * MAPCHIP_HEIGHT)) {
			EnemySpawner_Create({ 150.0f * MAPCHIP_WIDTH,80.0f * MAPCHIP_HEIGHT }, ENEMY_TYPE_BIRD, 0.0, 1.0, 3);

			g_IsSpawned[2] = true;
		}
		if (!g_IsSpawned[3] && CheckEnemyTriggerArea(145 * MAPCHIP_WIDTH, 151 * MAPCHIP_WIDTH, 40 * MAPCHIP_HEIGHT, 63 * MAPCHIP_HEIGHT)) {
			EnemySpawner_Create({ 110.0f * MAPCHIP_WIDTH,55.0f * MAPCHIP_HEIGHT }, ENEMY_TYPE_SLIME_GREEN, 0.0, 0.5, 3,true);
			EnemySpawner_Create({ 108.0f * MAPCHIP_WIDTH,55.0f * MAPCHIP_HEIGHT }, ENEMY_TYPE_SLIME_ORANGE, 0.0, 0.5, 3,true);
			g_IsSpawned[3] = true;
		}
		if (!g_IsSpawned[4] && CheckEnemyTriggerArea(119 * MAPCHIP_WIDTH, 127 * MAPCHIP_WIDTH, 0 * MAPCHIP_HEIGHT, 32 * MAPCHIP_HEIGHT)) {
			EnemySpawner_Create({ 165.0f * MAPCHIP_WIDTH,20.0f * MAPCHIP_HEIGHT }, ENEMY_TYPE_SLIME_GREEN, 0.0, 0.5, 3);
			EnemySpawner_Create({ 167.0f * MAPCHIP_WIDTH,20.0f * MAPCHIP_HEIGHT }, ENEMY_TYPE_SLIME_ORANGE, 0.0, 0.5, 3);
			EnemySpawner_Create({ 165.0f * MAPCHIP_WIDTH,15.0f * MAPCHIP_HEIGHT }, ENEMY_TYPE_BIRD, 0.0, 0.5, 3);
			g_IsSpawned[4] = true;
		}
		if (!g_IsSpawned[5] && CheckEnemyTriggerArea(210 * MAPCHIP_WIDTH, 220 * MAPCHIP_WIDTH, 0 * MAPCHIP_HEIGHT, 32 * MAPCHIP_HEIGHT)) {
			EnemySpawner_Create({ 260.0f * MAPCHIP_WIDTH,20.0f * MAPCHIP_HEIGHT }, ENEMY_TYPE_SLIME_GREEN, 0.0, 0.5, 5);
			EnemySpawner_Create({ 262.0f * MAPCHIP_WIDTH,20.0f * MAPCHIP_HEIGHT }, ENEMY_TYPE_SLIME_ORANGE, 0.0, 0.5, 5);
			EnemySpawner_Create({ 262.0f * MAPCHIP_WIDTH,15.0f * MAPCHIP_HEIGHT }, ENEMY_TYPE_BIRD, 0.0, 0.5, 5);

			EnemySpawner_Create({ 180.0f * MAPCHIP_WIDTH,20.0f * MAPCHIP_HEIGHT }, ENEMY_TYPE_SLIME_GREEN, 0.0, 0.5, 5,true);
			EnemySpawner_Create({ 180.0f * MAPCHIP_WIDTH,20.0f * MAPCHIP_HEIGHT }, ENEMY_TYPE_SLIME_ORANGE, 0.0, 0.5, 5,true);
			EnemySpawner_Create({ 180.0f * MAPCHIP_WIDTH,15.0f * MAPCHIP_HEIGHT }, ENEMY_TYPE_BIRD, 0.0, 0.5, 5,true);
			g_IsSpawned[5] = true;
		}

		//ボス戦スタート
		if (!g_IsBossBattle && CheckEnemyTriggerArea(279 * MAPCHIP_WIDTH, 298 * MAPCHIP_WIDTH, 32 * MAPCHIP_HEIGHT, 40 * MAPCHIP_HEIGHT)) {
			Boss_ActiveSwitch();
			g_IsBossBattle=true;
			g_IsCheckpoint = true;

			BossUI_StartAnim();

			//ボス戦BGMへ切り替え
			StopAudio(g_BgmId);
			PlayAudio(g_BossBgmId, true);
		}

		//当たり判定
		hitJudgementAttackVSEnemy();
		hitJudgementPlayerVSEnemy();
		hitJudgementBulletVSEnemy();
		hitJudgementPlayerVSEnemyBullet();
		//hitJudgementBulletVSMap();

		hitJudgementPlayerVSBoss();
		hitJudgementBulletVSBoss();
		hitJudgementAttackVSBoss();
		hitJudgementPlayerVSKingsDrop();


		//プレイヤーのHPチェック
		if ((!g_GameEnd && Runner_IsDead())|| KeyLogger_IsTrigger(KK_R)) {
			Runner_Destroy();
			Particle_Create(ParticleType::PLAYER_DEATH, { Runner_GetPosition().x + RUNNER_WIDTH / 2.0f,Runner_GetPosition().y + RUNNER_HEIGHT / 2.0f });

			//死んだ座標を保存
			Game_SetRunnerPosition(Runner_GetPosition());

			Fade_Start(1.0, true);
			g_GameEnd = true;
		}

		if (g_GameEnd && Fade_GetState() == FADE_STATE_FINISHED_OUT) {
			Scene_Change(SCENE_RESULT);
			g_GameStart = false;
		}

		Effect_Update();
		Particle_Update(elapsed_time);
		if (!Game_IsPaused()) {
			PlayerUI_Update(elapsed_time);
		}
	}
}

void Game_Draw(){
	BG_Draw();
	Enemy_Draw();
	Runner_Draw();
	Map_Draw();
	Particle_Draw();
	EnemyBullet_Draw();
	Bullet_Draw();
	Effect_Draw();
	if (g_IsBossBattle) {
		Boss_Draw();
		BossUI_Draw();
	}
	if (!Game_IsPaused()) {
		PlayerUI_Draw();
	}
}

bool CheckEnemyTriggerArea(int min_x, int max_x, int min_y, int max_y){
	XMFLOAT2 player_position = Runner_GetPosition();
	if (player_position.x >= (float)min_x && player_position.x <= (float)max_x &&
		player_position.y >= (float)min_y && player_position.y <= (float)max_y) return true;

	return false;
}

void Game_SetCheckPoint(bool isCheckpoint){
	g_IsCheckpoint = isCheckpoint;
}

void Game_SetRunnerPosition(const XMFLOAT2& position) {
	g_PrevRunner_position = position;
}


void hitJudgementBulletVSEnemy(){
	for (int bi = 0; bi < BULLET_MAX;bi++) {
		
		if (!Bullet_IsEnable(bi)) continue;

		//弾一発につき全てのエネミーと当たり判定を見る
		for (int ei = 0;ei < ENEMY_MAX;ei++) {
			if (!Enemy_IsEnable(ei)) continue;

			if (Get_EnemyInvincibleTime(ei) > 0.0f) continue;

			if (Collision_IsOverlapCircle(Bullet_GetCollision(bi), Enemy_GetCollision(ei))) {
				//当たっていたら
				PlayAudio(g_BulletHitSEId);
				Bullet_Destroy(bi);
				Enemy_Damage(ei);
				Set_EnemyInvincibleTime(ei, 0.2f);
			}
		}
	}
}

void hitJudgementPlayerVSEnemy(){
	if (!Runner_IsEnable()) return;

	//プレイヤーと全てのエネミーと当たり判定を見る
	for (int ei = 0;ei < ENEMY_MAX;ei++) {
		if (!Enemy_IsEnable(ei)) continue;
		if (Collision_IsOverlapCircle(Runner_GetCollision(), Enemy_GetCollision(ei))) {
			//当たっていたら
			if (Get_EnemyHp(ei) >= 0.0f) {
				Runner_Damage(1.0f);
				Enemy_Destroy(ei);
			}
		}
	}
}

void hitJudgementPlayerVSEnemyBullet(){
	
	if (!Runner_IsEnable()) return;

	//プレイヤーと全てのエネミーの弾と当たり判定を見る
	for (int eb = 0;eb < BULLET_MAX;eb++) {
		if (Bullet_WillExplosion(eb)) continue;

		if (!EnemyBullet_IsEnable(eb)) continue;

		if (Collision_IsOverlapCircle(Runner_GetCollision(), EnemyBullet_GetCollision(eb))) {
			//当たっていたら
			Boss_IsReinforced() ? Runner_Damage(2.0f) : Runner_Damage(1.0f);
			EnemyBullet_Destroy(eb);
		}
	}
	
}

void hitJudgementAttackVSEnemy(){
	OBB attack_obb = Runner_GetAttackCollision();
	if (attack_obb.half_extent.x <= 0) return;

	for (int ei = 0; ei < ENEMY_MAX; ei++) {
		if (!Enemy_IsEnable(ei)) continue;

		if (Get_EnemyInvincibleTime(ei) > 0.0f) continue;

		if (Collision_IsOverlapOBBVSCircle(attack_obb, Enemy_GetCollision(ei))) {
			Enemy_Damage(ei);
			PlayAudio(g_SwordHitSEId);
			Set_EnemyInvincibleTime(ei, 0.2f);
		}
	}
}

void hitJudgementBulletVSMap(){
	for (int bi = 0; bi < BULLET_MAX; bi++) {

		if (!Bullet_IsEnable(bi)) continue;

		if (Map_hitJudgementCircleVSMap(Bullet_GetCollision(bi))) {
			Bullet_Destroy(bi);
		}
	}
	for (int ebi = 0; ebi < BULLET_MAX; ebi++) {

		if (!EnemyBullet_IsEnable(ebi)) continue;

		if (Map_hitJudgementCircleVSMap(EnemyBullet_GetCollision(ebi))) {
			EnemyBullet_Destroy(ebi);
		}
	}
}

void hitJudgementPlayerVSBoss(){
	if (!Boss_IsAlive()) return;

	if (Collision_IsOverlapCircleVSBox(Boss_GetBoxCollision(), Runner_GetCollision())) {
		if (Boss_IsReinforced()) Get_BossState() == BOSS_ATTACK ? Runner_Damage(2.0f) : Runner_Damage(1.0f);
		else Runner_Damage(1.0f);
	}
}

void hitJudgementBulletVSBoss(){
	if (!Boss_IsAlive()) return;

	if (Get_BossInvincibleTime() > 0.0f) return;

	for (int bi = 0; bi < BULLET_MAX; bi++) {
		if (!Bullet_IsEnable(bi)) continue;

		if (Collision_IsOverlapCircleVSBox(Boss_GetBoxCollision(), Bullet_GetCollision(bi))) {
			PlayAudio(g_BulletHitSEId);
			Boss_Damage(1);
			Bullet_Destroy(bi);
		}
	}
}

void hitJudgementAttackVSBoss(){
	if (!Boss_IsAlive()) return;

	OBB attack_obb = Runner_GetAttackCollision();
	if (attack_obb.half_extent.x <= 0) return;

	if (Get_BossInvincibleTime() > 0.0f) return;

	if (Collision_IsOverlapOBBVSBox(attack_obb, Boss_GetBoxCollision())) {
		PlayAudio(g_SwordHitSEId);
		if (Runner_GetState() == STATE_STRONGATTACKEND)Boss_Damage(3);
		else Boss_Damage(1);
	}
}

void hitJudgementPlayerVSKingsDrop(){
	if (Get_BossState() != BOSS_KINGS_EXPLOSION) return;

	if (Collision_IsOverlapCircle(Runner_GetCollision(), BossKingsDrop_GetCollision())) {
		Runner_Damage(5.0f);
	}
}
