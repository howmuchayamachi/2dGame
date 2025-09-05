/*==============================================================================

  スコア表示[score.h]
														 Author : Harada Ren
														 Date   : 2025/07/09
--------------------------------------------------------------------------------

==============================================================================*/
#ifndef SCORE_H
#define SCORE_H

//表示する左上座標と、使う桁数
void Score_Initialize(float x, float y, int digit); //他にもゼロ埋め、左寄せなど
void Score_Finalize();
void Score_Update();
void Score_Draw();

unsigned int Score_GetScore();
void Score_AddScore(int score);
void Score_Reset();

#endif //SCORE_H