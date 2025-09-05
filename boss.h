/*==============================================================================

   ボス管理[boss.h]
														 Author : Harada Ren
														 Date   : 2025/08/11
--------------------------------------------------------------------------------

==============================================================================*/
#ifndef BOSS_H
#define BOSS_H

#include "collision.h"

static constexpr int BOSS_MAXHP = 60;

enum BossState {
	BOSS_STANDBY, //待機
	BOSS_MOVE, //移動
	BOSS_WARP_BEFORE, //ワープ前
	BOSS_WARP, //ワープ
	BOSS_WARP_AFTER, //ワープ後
	BOSS_ATTACK, //突進
	BOSS_SPELL, //火の弾発射
	BOSS_SUMMON, //雑魚敵召喚
	BOSS_CHARGE, //半分切った時のチャージ演出
	BOSS_KINGS_DROP, //必殺技
	BOSS_KINGS_EXPLOSION, //爆発
	BOSS_END, //死亡演出
	BOSS_STATEMAX
};

void Boss_Initialize();
void Boss_Finalize();
void Boss_Update(double elapsed_time);
void Boss_Draw();

Box Boss_GetBoxCollision();

void Boss_Damage(int damage);

void Boss_ActiveSwitch();

bool Boss_IsAlive();

int Boss_GetHp();

void ChangeBossState(BossState newstate);

void Set_BossInvincibleTime(float time);
float Get_BossInvincibleTime();

DirectX::XMFLOAT2 Boss_GetPosition();

bool Boss_IsReinforced();

BossState Get_BossState();

void Set_KingsExplosionPosition(DirectX::XMFLOAT2 position);
DirectX::XMFLOAT2 Get_KingsExplosionPosition();

Circle BossKingsDrop_GetCollision();

#endif //BOSS_H