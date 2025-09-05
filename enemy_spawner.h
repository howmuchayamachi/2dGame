/*==============================================================================

   ìGÇÃî≠ê∂êßå‰ [enemy_spawner.h]
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

//î≠ê∂à íu, id, ÉgÉäÉKÅ[(î≠ê∂éûä‘Ç‚î≠ê∂Ç∑ÇÈà íuÇ»Ç«) , î≠ê∂ä‘äu, å¬êî
void EnemySpawner_Create(const DirectX::XMFLOAT2& position, EnemyTypeID id, double spawn_time, double spawn_rate, int spawn_count, bool gotoright = false);


#endif //ENEMY_SPAWNER_H