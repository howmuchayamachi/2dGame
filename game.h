/*==============================================================================

   ÉQÅ[ÉÄñ{ëÃ[game.h]
														 Author : Harada Ren
														 Date   : 2025/06/27
--------------------------------------------------------------------------------

==============================================================================*/
#ifndef GAME_H
#define GAME_H

#include <DirectXMath.h>

void Game_Initialize();
void Game_Finalize();

void Game_Update(double elapsed_time);
void Game_Draw();

bool CheckEnemyTriggerArea(int min_x, int max_x, int min_y, int max_y);

void Game_SetCheckPoint(bool isCheckpoint);

void Game_SetPlayerPosition(const DirectX::XMFLOAT2& position);

void hitJudgementBulletVSEnemy();
void hitJudgementPlayerVSEnemy();
void hitJudgementPlayerVSEnemyBullet();
void hitJudgementAttackVSEnemy();
void hitJudgementBulletVSMap();
void hitJudgementPlayerVSBoss();
void hitJudgementBulletVSBoss();
void hitJudgementAttackVSBoss();
void hitJudgementPlayerVSKingsDrop();

#endif //GAME_H