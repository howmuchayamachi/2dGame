/*==============================================================================

   �e���� [bullet.h]
														 Author : Harada Ren
														 Date   : 2025/07/01
--------------------------------------------------------------------------------

==============================================================================*/
#ifndef BULLET_H
#define BULLET_H

#include <DirectXMath.h>
#include "Collision.h"

static constexpr unsigned int BULLET_MAX = 1024;

void Bullet_Initialize();
void Bullet_Finalize();

void Bullet_Update(double elapsed_time);
void Bullet_Draw();
void EnemyBullet_Draw();

void Bullet_Create(const DirectX::XMFLOAT2& position, const DirectX::XMFLOAT2 target_position);
//�e�̎�ށA�����ځA��������ς���ꍇ�͈����𑝂₷

void EnemyBullet_Create(const DirectX::XMFLOAT2& position, const DirectX::XMFLOAT2 target_position, bool isBossReinforced = false, bool isKingsDrop = false);

bool Bullet_IsEnable(int index);//�L����
bool EnemyBullet_IsEnable(int index);//�L����

Circle Bullet_GetCollision(int index);

Circle EnemyBullet_GetCollision(int index);


void Bullet_Destroy(int index);
void EnemyBullet_Destroy(int index);

bool Bullet_WillExplosion(int index);


#endif //BULLET_H