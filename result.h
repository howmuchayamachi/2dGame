/*==============================================================================

  ÉäÉUÉãÉgêßå‰[result.h]
														 Author : Harada Ren
														 Date   : 2025/07/11
--------------------------------------------------------------------------------

==============================================================================*/
#ifndef RESULT_H
#define RESULT_H

enum ResultMenu {
	RESULT_CONTINUE,
	RESULT_GOTOTITLE,
	RESULT_EXIT,
	RESULT_MAX
};

void Result_Initialize();
void Result_Finalize();
void Result_Update();
void Result_Draw();

#endif //RESULT_H