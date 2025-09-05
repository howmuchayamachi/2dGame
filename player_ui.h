/*====================================================================================

ÉvÉåÉCÉÑÅ[UIä«óù[player_ui.h]

																Author	: Harada Ren
																Date	: 2025/08/07
--------------------------------------------------------------------------------------

======================================================================================*/
#ifndef PLAYER_UI_H
#define PLAYER_UI_H

void PlayerUI_Initialize();
void PlayerUI_Finalize();
void PlayerUI_Update(double elapsed_time);
void PlayerUI_Draw();

bool isHpDecrease();
bool isMpDecrease();

void PlayerUI_Switch();

#endif //PLAYER_UI_H
