/*====================================================================================

�L�[�{�[�h���͂̋L�^[key_logger.h]

																Author	: Harada Ren
																Date	: 2025/06/27
--------------------------------------------------------------------------------------

======================================================================================*/
#ifndef KEY_LOGGER_H
#define KEY_LOGGER_H

#include "keyboard.h"

void KeyLogger_Initialize();
void KeyLogger_Update();
bool KeyLogger_IsPressed(Keyboard_Keys key); //�����Ă����
bool KeyLogger_IsTrigger(Keyboard_Keys key); //�������u��
bool KeyLogger_IsReleased(Keyboard_Keys key); //�������u��

#endif //KEY_LOGGER_H