/*==============================================================================

  É^ÉCÉgÉãêßå‰[title.h]
														 Author : Harada Ren
														 Date   : 2025/07/11
--------------------------------------------------------------------------------

==============================================================================*/
#ifndef TITLE_H
#define TITLE_H

enum TitleMenu {
	TITLE_GAMESTART,
	TITLE_VOLUME,
	TITLE_EXIT,
	TITLE_MAX
};

void Title_Initialize();
void Title_Finalize();
void Title_Update();
void Title_Draw();

#endif //TITLE_H