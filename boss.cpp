/*==============================================================================

   ボス管理[boss.cpp]
														 Author : Harada Ren
														 Date   : 2025/08/11
--------------------------------------------------------------------------------

==============================================================================*/
#include "boss.h"
#include "texture.h"
#include "sprite.h"
#include "sprite_anim.h"
#include <DirectXMath.h>
using namespace DirectX;
#include "map.h"
#include "runner.h"
#include <stdlib.h>
#include <time.h>
#include "bullet.h"
#include "enemy_spawner.h"
#include "fade.h"
#include "scene.h"
#include "Audio.h"
#include "effect.h"
#include "particle.h"

static int g_BossStandbyTexId = -1;
static int g_BossAttackTexId = -1;
static int g_BossMoveTexId = -1;

static int g_KingsDropRangeTexId = -1;

static int AnimId_BossStandby = -1;
static int AnimId_BossAttack = -1;
static int AnimId_BossMove = -1;

static int PlayId_BossStandby = -1;
static int PlayId_BossAttack = -1;
static int PlayId_BossMove = -1;

static int g_BossExplosionTexId = -1;
static int AnimId_BossExplosion = -1;
static int PlayId_BossExplosion = -1;

static constexpr float BOSS_DISPLAY_WIDTH = 200.0f;
static constexpr float BOSS_DISPLAY_HEIGHT = 200.0f;
static constexpr float BOSS_WIDTH = 50.0f;
static constexpr float BOSS_HEIGHT = 100.0f;

static bool g_IsBossBattleEnd = false;

static float g_ShotRate = 0.4f;
static float g_ShotTimer = g_ShotRate;

static float g_SummonRate = 0.4f;
static float g_SummonTimer = g_SummonRate;

static float g_GatherPowerRate = 0.5f;
static float g_GatherPowerTimer = g_GatherPowerRate;

static bool g_BossWantAttack = false;
static int g_BossAttackDirection = -1;

static int g_BossWarpSEId = -1;
static int g_BossAttackSEId = -1;
static int g_BossBulletSEId = -1;
static int g_BossSummonSEId = -1;

static int g_BossKingsDropChargeSEId = -1;
static int g_BossKingsDropSEId = -1;
static int g_BossKingsExplosionSEId = -1;

static int g_ExplosionAudioId = -1;
static int g_BossEndExplosionAudioId = -1;

static XMFLOAT2 g_KingsExplosionPosition{};

static constexpr int EXPLOSION_MAXCOUNT = 5;
static int expl_count = 0;

static float explosion_alpha = 0.0f;

static constexpr XMINT2 Boss_minPoint{ 185 * MAPCHIP_WIDTH,48 * MAPCHIP_HEIGHT };
static constexpr XMINT2 Boss_maxPoint{ 293 * MAPCHIP_WIDTH,88 * MAPCHIP_HEIGHT };

struct Boss {
	XMFLOAT2 position;
	XMFLOAT2 velocity;
	int hp; //現在のHP
	int maxHp; //最大HP
	bool isActive; //使っているかどうか
	bool isDamage; //ダメージを受けたか
	BossState state;
	BossState prev_AttackState; //前攻撃したステート
	bool isFacingRight; //右を向いているか
	Box collision;
	float InvincibleTime; //無敵時間
	float stateTimer; //一つのステートに使う時間
	float alpha; //透明度
	bool isReinforce; //HPが半分以下だったら攻撃を強化
	bool isSuperReinforce; //もっとピンチだったら攻撃をもっと強化
	bool isDroppedKingsdrop;

};

static Boss g_Boss{};

void Boss_Initialize() {
	g_Boss.position = { 240.0f * MAPCHIP_WIDTH,80.0f * MAPCHIP_HEIGHT };
	g_Boss.isActive = false;
	g_Boss.maxHp = BOSS_MAXHP;
	g_Boss.hp = g_Boss.maxHp;
	g_Boss.state = BOSS_STANDBY;
	g_Boss.prev_AttackState = BOSS_ATTACK;
	g_Boss.collision = { {BOSS_DISPLAY_WIDTH / 2.0f,BOSS_DISPLAY_HEIGHT / 2.0f},BOSS_WIDTH,BOSS_HEIGHT };
	g_Boss.InvincibleTime = 0.0f;
	g_Boss.stateTimer = 0.0f;
	g_Boss.isReinforce = false;
	g_Boss.isSuperReinforce = false;
	g_Boss.isDroppedKingsdrop = false;

	g_BossStandbyTexId = Texture_Load(L"resource/texture/boss/boss_standby.png");
	AnimId_BossStandby = SpriteAnim_RegisterPattern(g_BossStandbyTexId, 4, 2, 0.1, { 64,64 }, { 0,0 });
	PlayId_BossStandby = SpriteAnim_CreatePlayer(AnimId_BossStandby);

	g_BossAttackTexId = Texture_Load(L"resource/texture/boss/boss_attack.png");;
	AnimId_BossAttack = SpriteAnim_RegisterPattern(g_BossAttackTexId, 4, 2, 0.1, { 78,78 }, { 0,0 });
	PlayId_BossAttack = SpriteAnim_CreatePlayer(AnimId_BossAttack);

	g_BossMoveTexId = Texture_Load(L"resource/texture/boss/boss_move.png");
	AnimId_BossMove = SpriteAnim_RegisterPattern(g_BossMoveTexId, 6, 3, 0.1, { 64,64 }, { 0,0 });
	PlayId_BossMove = SpriteAnim_CreatePlayer(AnimId_BossMove);

	g_BossExplosionTexId = Texture_Load(L"resource/texture/explosion2.png");
	AnimId_BossExplosion = SpriteAnim_RegisterPattern(g_BossExplosionTexId, 7, 7, 0.1, { 300,400 }, { 0,0 });
	PlayId_BossExplosion = SpriteAnim_CreatePlayer(AnimId_BossExplosion);

	g_KingsDropRangeTexId = Texture_Load(L"resource/texture/dangerzone.png");

	g_BossWantAttack = false;
	g_BossAttackDirection = -1;

	g_IsBossBattleEnd = false;

	g_BossWarpSEId = LoadAudio("resource/audio/warp.wav");
	g_BossAttackSEId = LoadAudio("resource/audio/boss_attack.wav");
	g_BossBulletSEId = LoadAudio("resource/audio/gun01.wav");
	g_BossSummonSEId = LoadAudio("resource/audio/summon.wav");

	g_BossKingsDropChargeSEId = LoadAudio("resource/audio/kingsdropcharge.wav");
	g_BossKingsDropSEId = LoadAudio("resource/audio/kingsdrop.wav");
	g_BossKingsExplosionSEId = LoadAudio("resource/audio/kingsexplosion.wav");

	g_ExplosionAudioId = LoadAudio("resource/audio/explosion01.wav");
	g_BossEndExplosionAudioId = LoadAudio("resource/audio/boss_explosion.wav");


	expl_count = 0;

	explosion_alpha = 0.0f;

	g_ShotRate = 0.4f;
	g_ShotTimer = g_ShotRate;

	g_SummonRate = 0.4f;
	g_SummonTimer = g_SummonRate;

	srand((unsigned int)time(NULL));
}

void Boss_Finalize() {
}

void Boss_Update(double elapsed_time) {
	if (g_Boss.InvincibleTime > 0.0f) {
		g_Boss.InvincibleTime -= (float)elapsed_time;
	}

	g_Boss.stateTimer += (float)elapsed_time;

	XMFLOAT2 playerPosition = Runner_GetPosition();

	switch (g_Boss.state) {
	case BOSS_STANDBY: {
		g_Boss.isFacingRight = playerPosition.x > g_Boss.position.x + (float)(BOSS_WIDTH / 2);

		float BossSTimer = g_Boss.isReinforce ? 0.5f : 1.0f;
		if (g_Boss.stateTimer > BossSTimer) {
			int attackType = rand() % 2 + 1;
			//移動orワープ
			ChangeBossState((BossState)attackType);
		}
		break;
	}

	case BOSS_MOVE: {
		g_Boss.isFacingRight = playerPosition.x > g_Boss.position.x + (float)(BOSS_WIDTH / 2);

		XMFLOAT2 playerPos = Runner_GetPosition();
		float dirx = (playerPos.x > g_Boss.position.x) ? 1.0f : -1.0f;
		float horizontal_move = g_Boss.position.x + dirx * 300.0f * (float)elapsed_time;
		if (horizontal_move > Boss_minPoint.x && horizontal_move < Boss_maxPoint.x) {
			g_Boss.position.x = horizontal_move;
		}

		float diry = (playerPos.y > g_Boss.position.y) ? 1.0f : -1.0f;
		float vertical_move = g_Boss.position.y + diry * 100.0f * (float)elapsed_time;
		if (vertical_move > Boss_minPoint.y && vertical_move < Boss_maxPoint.y) {
			g_Boss.position.y = vertical_move;
		}

		float BossSTimer = g_Boss.isReinforce ? 1.0f : 2.0f;
		if (g_Boss.stateTimer > BossSTimer) {

			//攻撃or魔法or召喚
			ChangeBossState(Choose_BossAttack(g_Boss.prev_AttackState));
		}
		break;
	}

	case BOSS_WARP_BEFORE:
		g_Boss.alpha = 1.0f - g_Boss.stateTimer * 2.0f;
		if (g_Boss.alpha < 0.0f) g_Boss.alpha = 0.0f;
		if (g_Boss.alpha <= 0.0f) {
			ChangeBossState(BOSS_WARP);
		}
		break;

	case BOSS_WARP:
	{
		XMFLOAT2 WarpForwardPos;

		//通常ワープ
		if (!g_BossWantAttack) {
			WarpForwardPos.x = Runner_GetPosition().x + (float)((rand() % 800) - 400);
			WarpForwardPos.y = Runner_GetPosition().y - (float)(rand() % 600);

			//壁にめり込まないように
			if (WarpForwardPos.x < Boss_minPoint.x) {
				WarpForwardPos.x = (float)Boss_minPoint.x;
			}
			if (WarpForwardPos.x > Boss_maxPoint.x) {
				WarpForwardPos.x = (float)Boss_maxPoint.x;
			}
			if (WarpForwardPos.y < Boss_minPoint.y) {
				WarpForwardPos.y = (float)Boss_minPoint.y;
			}
			if (WarpForwardPos.y > Boss_maxPoint.y) {
				WarpForwardPos.y = (float)Boss_maxPoint.y;
			}
		}
		//突進攻撃時ワープ
		else {
			//プレイヤーの左右どちらかにワープする
			float PlayerPosX = Runner_GetPosition().x;
			int dirx = rand() % 2 == 1 ? 1 : -1;
			WarpForwardPos.x = PlayerPosX + dirx * 900.0f;
			WarpForwardPos.y = Runner_GetPosition().y - BOSS_HEIGHT;

			//壁にめり込んでいたら逆の位置にする
			//y座標はプレイヤーの位置で固定のため調べない
			if (WarpForwardPos.x < Boss_minPoint.x) {
				WarpForwardPos.x = PlayerPosX + 1000.0f;
			}
			if (WarpForwardPos.x > Boss_maxPoint.x) {
				WarpForwardPos.x = PlayerPosX - 1000.0f;
			}
		}

		PlayAudio(g_BossWarpSEId);
		g_Boss.position = WarpForwardPos;
		ChangeBossState(BOSS_WARP_AFTER);

		g_Boss.isFacingRight = playerPosition.x > g_Boss.position.x + (float)(BOSS_WIDTH / 2);
	}
	break;

	case BOSS_WARP_AFTER:
		g_Boss.alpha = g_Boss.stateTimer * 2.0f;
		if (g_Boss.alpha > 1.0f) g_Boss.alpha = 1.0f;

		if (g_Boss.alpha >= 1.0f) {
			if (!g_BossWantAttack) {
				
				//攻撃or魔法or召喚
				ChangeBossState(Choose_BossAttack(g_Boss.prev_AttackState));
			}
			else {
				PlayAudio(g_BossAttackSEId);
				g_BossAttackDirection = g_Boss.isFacingRight ? 1 : -1;
				ChangeBossState(BOSS_ATTACK);
			}
		}
		break;

	case BOSS_ATTACK:
		//プレイヤーの横ラインにワープさせる
		if (!g_BossWantAttack) {
			g_BossWantAttack = true;
			ChangeBossState(BOSS_WARP_BEFORE);
		}
		//突進
		else {
			if (!g_Boss.isReinforce) {
				g_Boss.position.x += g_BossAttackDirection * 1000.0f * (float)elapsed_time;
			}
			else {
				if (!g_Boss.isSuperReinforce) {
					g_Boss.position.x += g_BossAttackDirection * 1500.0f * (float)elapsed_time;
				}
				else {
					g_Boss.position.x += g_BossAttackDirection * 2000.0f * (float)elapsed_time;
				}
			}

			//壁にめり込まないように
			if (g_Boss.position.x < Boss_minPoint.x) {
				g_Boss.position.x = (float)Boss_minPoint.x;
			}
			if (g_Boss.position.x > Boss_maxPoint.x) {
				g_Boss.position.x = (float)Boss_maxPoint.x;
			}

		}
		{
			double bossSTimer = g_Boss.isReinforce ? 1.0f : 2.0f;
			if (g_Boss.stateTimer > bossSTimer) {
				g_BossWantAttack = false;
				ChangeBossState(BOSS_WARP_BEFORE);
			}
		}

		g_Boss.prev_AttackState = BOSS_ATTACK;

		break;

	case BOSS_SPELL:
		g_ShotTimer -= (float)elapsed_time;
		if (g_ShotTimer <= 0.0f) {
			PlayAudio(g_BossBulletSEId);
			if (g_Boss.isFacingRight) EnemyBullet_Create({ g_Boss.position.x + BOSS_DISPLAY_WIDTH / 2.0f,g_Boss.position.y }, Runner_GetPosition(), g_Boss.isReinforce);
			else EnemyBullet_Create(g_Boss.position, Runner_GetPosition(), g_Boss.isReinforce);

			g_ShotTimer = g_ShotRate;
		}
		if (g_Boss.stateTimer > 2.0f) {
			g_ShotTimer = g_ShotRate;
			ChangeBossState(BOSS_STANDBY);
		}

		g_Boss.prev_AttackState = BOSS_SPELL;

		break;

	case BOSS_SUMMON:
		g_SummonTimer -= (float)elapsed_time;
		{
			if (g_SummonTimer <= 0.0f) {
				PlayAudio(g_BossSummonSEId);
				int enemy_type = rand() % 3;

				//召喚レート毎にランダムで2体召喚
				if (g_Boss.isFacingRight) {
					EnemySpawner_Create({ g_Boss.position.x + BOSS_DISPLAY_WIDTH / 2.0f,g_Boss.position.y }, (EnemyTypeID)enemy_type, 0.0, 0.0, 1);
					enemy_type = rand() % 3;
					EnemySpawner_Create({ g_Boss.position.x + BOSS_DISPLAY_WIDTH / 2.0f,g_Boss.position.y }, (EnemyTypeID)enemy_type, 0.1, 0.0, 1);
				}
				else {
					EnemySpawner_Create(g_Boss.position, (EnemyTypeID)enemy_type, 0.0, 0.0, 1);
					enemy_type = rand() % 3;
					EnemySpawner_Create(g_Boss.position, (EnemyTypeID)enemy_type, 0.1, 0.0, 1);
				}
				g_SummonTimer = g_SummonRate;
			}
		}


		if (g_Boss.stateTimer > 2.0f) {
			g_SummonTimer = g_SummonRate;
			ChangeBossState(BOSS_WARP_BEFORE);
		}

		g_Boss.prev_AttackState = BOSS_SUMMON;

		break;

	case BOSS_CHARGE:
		if (!g_Boss.isFacingRight) Particle_Create(ParticleType::GATHERING_POWER, { g_Boss.position.x,g_Boss.position.y + BOSS_HEIGHT });
		else Particle_Create(ParticleType::GATHERING_POWER, { g_Boss.position.x + BOSS_DISPLAY_WIDTH / 2.0f,g_Boss.position.y + BOSS_HEIGHT });

		if (g_Boss.stateTimer > 3.0) {
			ChangeBossState(BOSS_KINGS_DROP);
		}
		break;

	case BOSS_KINGS_DROP:
		if (g_Boss.stateTimer > 1.0 && !g_Boss.isDroppedKingsdrop) {
			PlayAudio(g_BossKingsDropSEId);
			g_Boss.isDroppedKingsdrop = true;
			Set_KingsExplosionPosition(Runner_GetPosition()); 
			if (!g_Boss.isFacingRight) EnemyBullet_Create({ g_Boss.position.x,g_Boss.position.y }, Get_KingsExplosionPosition(), true, true);
			else EnemyBullet_Create({ g_Boss.position.x + BOSS_DISPLAY_WIDTH / 2.0f,g_Boss.position.y }, Get_KingsExplosionPosition(), true, true);
		}

		if (g_Boss.isDroppedKingsdrop) {
			explosion_alpha += (float)(0.5 * elapsed_time);
			if (explosion_alpha > 0.5) explosion_alpha = 0.0f;
		}

		if (g_Boss.stateTimer > 4.0) {
			PlayAudio(g_BossKingsExplosionSEId);
			g_Boss.isDroppedKingsdrop = false;
			Map_StartShake(20.0f, 1.0);
			ChangeBossState(BOSS_KINGS_EXPLOSION);
		}
		break;

	case BOSS_KINGS_EXPLOSION:
		Particle_Create(ParticleType::KINGS_EXPLOSION, g_KingsExplosionPosition);

		if (g_Boss.stateTimer > 1.0) {
			g_Boss.InvincibleTime = 0.0f;
			ChangeBossState(BOSS_STANDBY);
		}

		g_Boss.prev_AttackState = BOSS_ATTACK;

		break;

	case BOSS_END:
	{
		g_ShotTimer -= (float)elapsed_time;

		if (g_ShotTimer <= 0.0f && expl_count < EXPLOSION_MAXCOUNT) {
			XMFLOAT2 explosion_pos = g_Boss.position;
			explosion_pos.x += (float)((rand() % (int)BOSS_DISPLAY_WIDTH) - (int)(BOSS_DISPLAY_WIDTH / 2.0f));
			explosion_pos.y += (float)((rand() % (int)BOSS_DISPLAY_HEIGHT) - (int)(BOSS_DISPLAY_HEIGHT / 2.0f));

			Effect_Create(explosion_pos);
			Particle_Create(ParticleType::BOSS_DEATH, { explosion_pos.x + BOSS_DISPLAY_WIDTH / 2.0f,explosion_pos.y + BOSS_DISPLAY_HEIGHT / 2.0f });
			PlayAudio(g_ExplosionAudioId);

			expl_count++;
			g_ShotTimer = g_ShotRate;
		}
	}

	if (g_Boss.stateTimer > 3.0f) {
		if (!g_IsBossBattleEnd) {
			g_IsBossBattleEnd = true;
			Fade_Start(1.0, true, { 1.0f,1.0f,1.0f });
		}
	}

	if (g_IsBossBattleEnd && FADE_STATE_FINISHED_OUT) {
		Scene_Change(SCENE_CLEAR);
	}
	break;

	default:
		break;
	}
}

void Boss_Draw() {
	if (!g_Boss.isActive) return;

	XMFLOAT2 offset = Map_GetWorldOffset();

	float screen_x = g_Boss.position.x - offset.x;
	float screen_y = g_Boss.position.y - offset.y;

	switch (g_Boss.state) {
	case BOSS_STANDBY:
		SpriteAnim_Draw(PlayId_BossStandby, screen_x, screen_y, BOSS_DISPLAY_WIDTH, BOSS_DISPLAY_HEIGHT, g_Boss.isFacingRight);
		break;

	case BOSS_MOVE:
		SpriteAnim_Draw(PlayId_BossMove, screen_x, screen_y, BOSS_DISPLAY_WIDTH, BOSS_DISPLAY_HEIGHT, g_Boss.isFacingRight);
		break;

	case BOSS_WARP_BEFORE:
		SpriteAnim_Draw(PlayId_BossStandby, screen_x, screen_y, BOSS_DISPLAY_WIDTH, BOSS_DISPLAY_HEIGHT, g_Boss.isFacingRight, { 1.0f,1.0f,1.0f,g_Boss.alpha });
		break;

	case BOSS_WARP:
		SpriteAnim_Draw(PlayId_BossStandby, screen_x, screen_y, BOSS_DISPLAY_WIDTH, BOSS_DISPLAY_HEIGHT, g_Boss.isFacingRight);
		break;

	case BOSS_WARP_AFTER:
		SpriteAnim_Draw(PlayId_BossStandby, screen_x, screen_y, BOSS_DISPLAY_WIDTH, BOSS_DISPLAY_HEIGHT, g_Boss.isFacingRight, { 1.0f,1.0f,1.0f,g_Boss.alpha });
		break;

	case BOSS_ATTACK:
		SpriteAnim_Draw(PlayId_BossMove, screen_x, screen_y, BOSS_DISPLAY_WIDTH, BOSS_DISPLAY_HEIGHT, g_Boss.isFacingRight);
		break;

	case BOSS_SPELL:
		SpriteAnim_Draw(PlayId_BossAttack, screen_x, screen_y, BOSS_DISPLAY_WIDTH, BOSS_DISPLAY_HEIGHT, g_Boss.isFacingRight);
		break;

	case BOSS_SUMMON:
		SpriteAnim_Draw(PlayId_BossAttack, screen_x, screen_y, BOSS_DISPLAY_WIDTH, BOSS_DISPLAY_HEIGHT, g_Boss.isFacingRight);
		break;

	case BOSS_CHARGE:
		SpriteAnim_Draw(PlayId_BossAttack, screen_x, screen_y, BOSS_DISPLAY_WIDTH, BOSS_DISPLAY_HEIGHT, g_Boss.isFacingRight);
		break;

	case BOSS_KINGS_DROP:
		if (g_Boss.stateTimer < 1.3f) {
			SpriteAnim_Draw(PlayId_BossAttack, screen_x, screen_y, BOSS_DISPLAY_WIDTH, BOSS_DISPLAY_HEIGHT, g_Boss.isFacingRight);
		}
		else {
			SpriteAnim_Draw(PlayId_BossStandby, screen_x, screen_y, BOSS_DISPLAY_WIDTH, BOSS_DISPLAY_HEIGHT, g_Boss.isFacingRight);
		}

		//危険範囲表示
		if (g_Boss.isDroppedKingsdrop) {
			float explosion_x = Get_KingsExplosionPosition().x - offset.x - 800.0f + RUNNER_WIDTH / 2.0f;
			float explosion_y = Get_KingsExplosionPosition().y - offset.y - 800.0f + RUNNER_HEIGHT / 2.0f;

			Sprite_Draw(g_KingsDropRangeTexId, explosion_x, explosion_y, 1600.0f, 1600.0f, { 1.0f,0.1f,0.1f,explosion_alpha});

		}
	
		break;

	case BOSS_KINGS_EXPLOSION:
#if defined(DEBUG)||defined(_DEBUG)
		//Collision_DebugDraw(BossKingsDrop_GetCollision(),{1.0f,0.0f,0.0f,1.0f});
#endif
		SpriteAnim_Draw(PlayId_BossStandby, screen_x, screen_y, BOSS_DISPLAY_WIDTH, BOSS_DISPLAY_HEIGHT, g_Boss.isFacingRight);
		break;

	case BOSS_END:
		break;

	default:
		break;
	}

#if defined(DEBUG)||defined(_DEBUG)
	//Collision_DebugDraw(Boss_GetBoxCollision());
#endif
}


Box Boss_GetBoxCollision() {
	float cx = g_Boss.position.x + g_Boss.collision.center.x;
	float cy = g_Boss.position.y + g_Boss.collision.center.y;

	return { {cx,cy},BOSS_WIDTH,BOSS_HEIGHT };
}

void Boss_Damage(int damage) {
	if (g_Boss.InvincibleTime > 0.0f) return;

	g_Boss.hp -= damage;

	Set_BossInvincibleTime(0.2f);

	if (!g_Boss.isReinforce && g_Boss.hp <= 2 * BOSS_MAXHP / 3) {
		PlayAudio(g_BossKingsDropChargeSEId);
		g_Boss.isReinforce = true;
		Set_BossInvincibleTime(10.0f);
		Map_StartShake(10.0f, 3.0);
		ChangeBossState(BOSS_CHARGE);
	}

	if (!g_Boss.isSuperReinforce && g_Boss.hp <= 1 * BOSS_MAXHP / 3) {
		PlayAudio(g_BossKingsDropChargeSEId);
		g_Boss.isSuperReinforce = true;
		Set_BossInvincibleTime(10.0f);
		g_ShotRate = 0.2f;
		g_SummonRate = 0.3f;
		Map_StartShake(10.0f, 3.0);
		ChangeBossState(BOSS_CHARGE);
	}

	if (g_Boss.hp <= 0) {
		g_Boss.isActive = false;
		g_ShotRate = 0.4f;
		PlayAudio(g_BossEndExplosionAudioId);

		ChangeBossState(BOSS_END);
	}
}

void Boss_ActiveSwitch() {
	g_Boss.isActive = true;
	g_Boss.hp = g_Boss.maxHp;
	Set_BossInvincibleTime(1.0f);
}

bool Boss_IsAlive() {
	return g_Boss.isActive;
}

int Boss_GetHp() {
	return g_Boss.hp;
}

void ChangeBossState(BossState newstate) {
	g_Boss.state = newstate;
	g_Boss.stateTimer = 0.0f;
}

void Set_BossInvincibleTime(float time) {
	g_Boss.InvincibleTime = time;
}

float Get_BossInvincibleTime() {
	return g_Boss.InvincibleTime;
}

DirectX::XMFLOAT2 Boss_GetPosition() {
	return g_Boss.position;
}

bool Boss_IsReinforced() {
	return g_Boss.isReinforce;
}

BossState Get_BossState() {
	return g_Boss.state;
}

BossState Choose_BossAttack(BossState prev_attack){
	if (prev_attack == BOSS_ATTACK) return BOSS_SPELL;
	else if (prev_attack == BOSS_SPELL) return BOSS_SUMMON;
	else if (prev_attack == BOSS_SUMMON) return BOSS_ATTACK;
	else return BOSS_STANDBY;
}

void Set_KingsExplosionPosition(DirectX::XMFLOAT2 position) {
	g_KingsExplosionPosition = position;
}

XMFLOAT2 Get_KingsExplosionPosition() {
	return g_KingsExplosionPosition;
}

Circle BossKingsDrop_GetCollision() {
	return { {g_KingsExplosionPosition.x + RUNNER_WIDTH / 2.0f,g_KingsExplosionPosition.y + RUNNER_HEIGHT / 2.0f},800.0f };
}
