/*==============================================================================

   ”wŒi‚Ì•\Ž¦ [bg.h]
														 Author : Harada Ren
														 Date   : 2025/07/16
--------------------------------------------------------------------------------

==============================================================================*/
#include "bg.h"
#include "texture.h"
#include "sprite.h"
#include "map.h"
#include "direct3d.h"
#include "DirectXMath.h"
using namespace DirectX;

static int g_BG01TexId = -1;
static int g_BG02TexId = -1;
static int g_BG03TexId = -1;
static int g_BG04TexId = -1;
static int g_BG05TexId = -1;

static float g_BgPositionX = 0.0f; // ”wŒi‚Ì•\Ž¦ˆÊ’u
static const float PARALLAX_SCALE_SKY = 0.2f;
static const float PARALLAX_SCALE_FOREST_BACK = 0.4f;
static const float PARALLAX_SCALE_FOREST_MID = 0.6f;
static const float PARALLAX_SCALE_FOREST_FRONT = 0.8f;
static const float PARALLAX_SCALE_GROUND = 0.5f;


void BG_Initialize(){
	g_BG01TexId = Texture_Load(L"resource/texture/bg/nightsky.jpg");
	g_BG02TexId = Texture_Load(L"resource/texture/bg/ground.png");
	g_BG03TexId = Texture_Load(L"resource/texture/bg/forest3.png");
	g_BG04TexId = Texture_Load(L"resource/texture/bg/forest2.png");
	g_BG05TexId = Texture_Load(L"resource/texture/bg/forest1.png");

	g_BgPositionX = 0.0f;
}

void BG_Finalize(){
}

void BG_Update(){
	g_BgPositionX = Map_GetWorldOffset().x;//offset.x;
}

void BG_Draw(){
	unsigned int screen_width = Direct3D_GetBackBufferWidth();
	unsigned int screen_height = Direct3D_GetBackBufferHeight();

	Sprite_Draw(g_BG01TexId, 0.0f, 0.0f, (int)(g_BgPositionX * PARALLAX_SCALE_SKY), 0, screen_width, screen_height);
	Sprite_Draw(g_BG02TexId, 0.0f, 0.0f, (int)(g_BgPositionX * PARALLAX_SCALE_GROUND), 0, screen_width, screen_height);
	Sprite_Draw(g_BG03TexId, 0.0f, 0.0f, (int)(g_BgPositionX * PARALLAX_SCALE_FOREST_BACK), 0, screen_width, screen_height,{0.1f,0.1f,0.1f,1.0f});
	Sprite_Draw(g_BG04TexId, 0.0f, 0.0f, (int)(g_BgPositionX * PARALLAX_SCALE_FOREST_MID), 0, screen_width, screen_height,{0.5f,0.5f,0.5f,1.0f});
	Sprite_Draw(g_BG05TexId, 0.0f, 0.0f, (int)(g_BgPositionX * PARALLAX_SCALE_FOREST_FRONT), 0, screen_width, screen_height,{0.9f,0.9f,0.9f,1.0f});

}
