/*====================================================================================

キーボード入力の記録[key_logger.h]

																Author	: Harada Ren
																Date	: 2025/06/27
--------------------------------------------------------------------------------------

======================================================================================*/
#ifndef KEY_LOGGER_H
#define KEY_LOGGER_H

#include "keyboard.h"

void KeyLogger_Initialize();
void KeyLogger_Update();
bool KeyLogger_IsPressed(Keyboard_Keys key); //押している間
bool KeyLogger_IsTrigger(Keyboard_Keys key); //押した瞬間
bool KeyLogger_IsReleased(Keyboard_Keys key); //離した瞬間

#endif //KEY_LOGGER_H