/*==============================================================================

   �G�̔������� [enemy_spawner.h]
														 Author : Harada Ren
														 Date   : 2025/07/02
--------------------------------------------------------------------------------

==============================================================================*/
#ifndef ENEMY_SPAWNER_H
#define ENEMY_SPAWNER_H

#include <DirectXMath.h>
#include "enemy.h"

void EnemySpawner_Initialize();
void EnemySpawner_Finalize();

void EnemySpawner_Update(double elapsed_time);

//�����ʒu, �G�l�~�[id, ��������, �����Ԋu, ��, �E������
void EnemySpawner_Create(const DirectX::XMFLOAT2& position, EnemyTypeID id, double spawn_time, double spawn_rate, int spawn_count, bool gotoright = false);


#endif //ENEMY_SPAWNER_H