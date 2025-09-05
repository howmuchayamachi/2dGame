/*==============================================================================

  ÉNÉäÉAâÊñ êßå‰[clear.h]
														 Author : Harada Ren
														 Date   : 2025/08/18
--------------------------------------------------------------------------------

==============================================================================*/
#ifndef CLEAR_H
#define CLEAR_H

enum ClearMenu {
	CLEAR_GOTOTITLE,
	CLEAR_EXIT,
	CLEAR_MAX
};

void Clear_Initialize();
void Clear_Finalize();
void Clear_Update();
void Clear_Draw();

#endif //CLEAR_H