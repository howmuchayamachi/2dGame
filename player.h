/*==============================================================================

   マップ移動キャラクター制御 [Player.h]
														 Author : Harada Ren
														 Date   : 2025/07/17
--------------------------------------------------------------------------------

==============================================================================*/
#ifndef player_H
#define player_H

#include <DirectXMath.h>
#include "collision.h"


static constexpr float Player_WIDTH = 128.0f;
static constexpr float Player_HEIGHT = 128.0f;

static constexpr float Player_MAXHP = 10;
static constexpr float Player_MAXMP = 10;


void Player_Initialize(const DirectX::XMFLOAT2& position);
void Runnner_Finalize();

void Player_Update(double elapsed_time);
void Player_Draw();

bool Player_IsEnable();//有効か
Circle Player_GetCollision();
Box Player_GetBoxCollision();
OBB Player_GetAttackCollision();

void Player_Destroy();

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

DirectX::XMFLOAT2 Player_GetPosition();

void Player_Damage(float damage);
float Player_GetHp();
float Player_GetMp();

bool Player_IsDead();

PLAYER_STATE Player_GetState();

bool Player_IsFacingRight();

#endif //player_H