/*==============================================================================

  É|Å[ÉYâÊñ êßå‰[pause.h]
														 Author : Harada Ren
														 Date   : 2025/08/20
--------------------------------------------------------------------------------

==============================================================================*/
#ifndef PAUSE_H
#define PAUSE_H

enum PauseMenu {
	PAUSE_BACKTOGAME,
	PAUSE_VOLUME,
	PAUSE_GOTOTITLE,
	PAUSE_MAX
};

void Pause_Initialize();
void Pause_Finalize();
void Pause_Update();
void Pause_Draw();

void Game_SetPause(bool is_paused);

bool Game_IsPaused();

#endif //PAUSE_H