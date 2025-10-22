/*==============================================================================

   敵の制御 [enemy.h]
														 Author : Harada Ren
														 Date   : 2025/07/02
--------------------------------------------------------------------------------

==============================================================================*/
#ifndef ENEMY_H
#define ENEMY_H

#include <DirectXMath.h>
#include "collision.h"

static constexpr unsigned int ENEMY_MAX = 1024;

void Enemy_Initialize();
void Enemy_Finalize();

void Enemy_Update(double elapsed_time);
void Enemy_Draw();

enum EnemyTypeID {
	ENEMY_TYPE_SLIME_GREEN, //緑スライム
	ENEMY_TYPE_SLIME_ORANGE, //橙スライム
	ENEMY_TYPE_BIRD, //鳥
	ENEMY_TYPE_MAX
};
void Enemy_Create(EnemyTypeID id,const DirectX::XMFLOAT2& position, bool GotoRight);

bool Enemy_IsEnable(int index); //有効か

Circle Enemy_GetCollision(int index);

Box Enemy_GetBoxCollision(int index);

DirectX::XMFLOAT2 Get_EnemyPosition(int index);

void Enemy_Destroy(int index);

void Enemy_Damage(int index);

void Set_EnemyInvincibleTime(int index, float time);

float Get_EnemyInvincibleTime(int index);

float Get_EnemyHp(int index);

DirectX::XMFLOAT2 Get_NearestTargetPosision(const DirectX::XMFLOAT2& player_position);

#endif //ENEMY_H