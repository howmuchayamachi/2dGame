/*==============================================================================

  �X�R�A�\��[score.h]
														 Author : Harada Ren
														 Date   : 2025/07/09
--------------------------------------------------------------------------------

==============================================================================*/
#ifndef SCORE_H
#define SCORE_H

//�\�����鍶����W�ƁA�g������
void Score_Initialize(float x, float y, int digit); //���ɂ��[�����߁A���񂹂Ȃ�
void Score_Finalize();
void Score_Update();
void Score_Draw();

unsigned int Score_GetScore();
void Score_AddScore(int score);
void Score_Reset();

#endif //SCORE_H