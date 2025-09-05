/*==============================================================================

   マップ移動キャラクター制御 [runner.h]
														 Author : Harada Ren
														 Date   : 2025/07/17
--------------------------------------------------------------------------------

==============================================================================*/
#ifndef RUNNER_H
#define RUNNER_H

#include <DirectXMath.h>
#include "collision.h"


static constexpr float RUNNER_WIDTH = 128.0f;
static constexpr float RUNNER_HEIGHT = 128.0f;


void Runner_Initialize(const DirectX::XMFLOAT2& position);
void Runnner_Finalize();

void Runner_Update(double elapsed_time);
void Runner_Draw();

bool Runner_IsEnable();//有効か
Circle Runner_GetCollision();
Box Runner_GetBoxCollision();
OBB Runner_GetAttackCollision();

void Runner_Destroy();

enum PLAYER_STATE {
	STATE_STOP,
	STATE_WALK,
	STATE_RUN,
	STATE_ATTACK,
	STATE_ATTACKEND,
	STATE_STRONGATTACK,
	STATE_STRONGATTACKEND,
	STATE_MAX
};

DirectX::XMFLOAT2 Runner_GetPosition();

//HP
static constexpr float RUNNER_MAXHP = 10;
//MP
static constexpr float RUNNER_MAXMP = 10;

void Runner_Damage(float damage);
float Runner_GetHp();
float Runner_GetMp();

bool Runner_IsDead();

PLAYER_STATE Runner_GetState();

bool Runner_IsFacingRight();

#endif //RUNNER_H