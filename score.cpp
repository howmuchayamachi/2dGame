/*==============================================================================

  スコア表示[score.cpp]
														 Author : Harada Ren
														 Date   : 2025/07/09
--------------------------------------------------------------------------------

==============================================================================*/
#include "score.h"
#include "texture.h"
#include "sprite.h"
#include "algorithm"

static unsigned int g_Score = 0;
static unsigned int g_ViewScore = 0;

static int g_Digit = 1;
static unsigned int g_CounterStop = 1;
static float g_x = 0.0f, g_y = 0.0f;
static int g_ScoreTexId = -1;

static constexpr int Score_Font_Width = 179;
static constexpr int Score_Font_Height = 185;


static void drawNumber(float x, float y, int number);

void Score_Initialize(float x,float y,int digit){
	g_Score = 0;
	g_ViewScore = 0;
	g_Digit = digit;
	g_x = x;
	g_y = y;

	//カンストの得点を作る
	for (int i = 0;i < digit;i++) {
		g_CounterStop *= 10;
	}
	g_CounterStop--;

	g_ScoreTexId = Texture_Load(L"resource/texture/number.png");
}

void Score_Finalize(){
}

void Score_Update(){
	g_ViewScore = std::min(g_ViewScore + 1, g_Score);
}

void Score_Draw(){
	unsigned int temp = std::min(g_ViewScore,g_CounterStop);
	for (int i = 0;i < g_Digit;i++) {
		int n = temp % 10;
		float x = g_x + (g_Digit - 1 - i)*Score_Font_Width;
		drawNumber(x, g_y, n);
		temp /= 10;
	}
}

unsigned int Score_GetScore(){
	return g_Score;
}

void Score_AddScore(int score){
	g_ViewScore = g_Score;
	g_Score += score;
}

void Score_Reset(){
	g_Score = 0;
}

void drawNumber(float x, float y, int number){
	Sprite_Draw(g_ScoreTexId, x, y, 32.0f, 64.0f, Score_Font_Width * number+30, 0, Score_Font_Width-70, Score_Font_Height);
}
