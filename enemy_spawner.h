/*==============================================================================

   敵の発生制御 [enemy_spawner.h]
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

//発生位置, エネミーid, 発生時間, 発生間隔, 個数, 右向きか
void EnemySpawner_Create(const DirectX::XMFLOAT2& position, EnemyTypeID id, double spawn_time, double spawn_rate, int spawn_count, bool gotoright = false);


#endif //ENEMY_SPAWNER_H