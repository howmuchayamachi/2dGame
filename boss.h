/*==============================================================================

   �{�X�Ǘ�[boss.h]
														 Author : Harada Ren
														 Date   : 2025/08/11
--------------------------------------------------------------------------------

==============================================================================*/
#ifndef BOSS_H
#define BOSS_H

#include "collision.h"

static constexpr int BOSS_MAXHP = 60;

enum BossState {
	BOSS_STANDBY, //�ҋ@
	BOSS_MOVE, //�ړ�
	BOSS_WARP_BEFORE, //���[�v�O
	BOSS_WARP, //���[�v
	BOSS_WARP_AFTER, //���[�v��
	BOSS_ATTACK, //�ːi
	BOSS_SPELL, //�΂̒e����
	BOSS_SUMMON, //�G���G����
	BOSS_CHARGE, //�����؂������̃`���[�W���o
	BOSS_KINGS_DROP, //�K�E�Z
	BOSS_KINGS_EXPLOSION, //����
	BOSS_END, //���S���o
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