/*====================================================================================

キーボード入力の記録[key_logger.cpp]

																Author	: Harada Ren
																Date	: 2025/06/27
--------------------------------------------------------------------------------------

======================================================================================*/
#include "key_logger.h"

static Keyboard_State g_PrevState{}; //1フレーム前の情報を取っておく
static Keyboard_State g_TriggerState{};
static Keyboard_State g_ReleasedState{};

void KeyLogger_Initialize(){
	Keyboard_Initialize();
}

void KeyLogger_Update(){
	const Keyboard_State* pState = Keyboard_GetState(); //現在のキーボードの状況取得
	LPBYTE pn = (LPBYTE)pState;
	LPBYTE pp = (LPBYTE)&g_PrevState;
	LPBYTE pt = (LPBYTE)&g_TriggerState;
	LPBYTE pr = (LPBYTE)&g_ReleasedState;

	for (int i = 0; i < sizeof(Keyboard_State); i++) {
		//1フレーム前と今の状態を比べる
		/*
		01->1
		10->0
		11->0
		00->0
		*/
		pt[i] = (pp[i] ^ pn[i]) & pn[i];
		/*
		01->0
		10->1
		11->0
		00->0
		*/
		pr[i]= (pp[i] ^ pn[i]) & pp[i];
	}

	g_PrevState = *pState; //現在の入力を過去の入力にする
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
