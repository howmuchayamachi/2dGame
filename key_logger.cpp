/*====================================================================================

�L�[�{�[�h���͂̋L�^[key_logger.cpp]

																Author	: Harada Ren
																Date	: 2025/06/27
--------------------------------------------------------------------------------------

======================================================================================*/
#include "key_logger.h"

static Keyboard_State g_PrevState{}; //1�t���[���O�̏�������Ă���
static Keyboard_State g_TriggerState{};
static Keyboard_State g_ReleasedState{};

void KeyLogger_Initialize(){
	Keyboard_Initialize();
}

void KeyLogger_Update(){
	const Keyboard_State* pState = Keyboard_GetState(); //���݂̃L�[�{�[�h�̏󋵎擾
	LPBYTE pn = (LPBYTE)pState;
	LPBYTE pp = (LPBYTE)&g_PrevState;
	LPBYTE pt = (LPBYTE)&g_TriggerState;
	LPBYTE pr = (LPBYTE)&g_ReleasedState;

	for (int i = 0; i < sizeof(Keyboard_State); i++) {
		pt[i] = (pp[i] ^ pn[i]) & pn[i];
		pr[i]= (pp[i] ^ pn[i]) & pp[i];
	}

	g_PrevState = *pState; //���݂̓��͂�ۑ�
}

bool KeyLogger_IsPressed(Keyboard_Keys key){
	return Keyboard_IsKeyDown(key);
}

bool KeyLogger_IsTrigger(Keyboard_Keys key){

	return Keyboard_IsKeyDown(key,&g_TriggerState);
}

bool KeyLogger_IsReleased(Keyboard_Keys key){

	return Keyboard_IsKeyDown(key, &g_ReleasedState);
}
