/*==============================================================================

   マップの表示[map.h]
														 Author : Harada Ren
														 Date   : 2025/07/16
--------------------------------------------------------------------------------

==============================================================================*/
#ifndef MAP_H
#define MAP_H

#include <DirectXMath.h>
#include "collision.h"
#include <vector>

static constexpr unsigned int MAPCHIP_WIDTH = 32;
static constexpr unsigned int MAPCHIP_HEIGHT = 32;

std::vector<uint32_t> LoadMapFromCSV(const char* filename, int* width, int* height);

void Map_Initialize();
void Map_Finalize();

void DrawLayer(int tex_id, const std::vector<uint32_t>& map_layer, int mx, int my,
	const DirectX::XMFLOAT2& local_offset,
	const DirectX::XMFLOAT4& color = { 1.0f,1.0f,1.0f,1.0f });

void Map_Draw();

void Map_SetWorldOffset(const DirectX::XMFLOAT2& world_offset);

bool Map_hitJudgementBoxVSMap(const Box& collision);

bool Map_hitJudgementCircleVSMap(const Circle& collision);

bool Map_hitJudgementBoxVSOneway(const Box& collision);

//ワールド座標からマップ座標へ変換する
int Map_GetWorldToMapX(float x);
int Map_GetWorldToMapY(float y);

//int Map_GetMapChip(int mx,int my);
int Map_GetMapChipFromLayer(const std::vector<uint32_t>& map_layer, int mx, int my);

DirectX::XMFLOAT2 Map_GetWorldOffset();

//mxはマップチップの幅 myは高さ
static void mapChipDraw(int tex_id, uint32_t mapChipId, int mx, int my, 
	const DirectX::XMFLOAT2& local_offset, int tileset_width_in_tiles,
	const DirectX::XMFLOAT4& color = { 1.0f,1.0f,1.0f,1.0f });

void Map_UpdateCamera(const DirectX::XMFLOAT2& player_pos , double elapsed_time);

void Map_StartShake(float fluctuation, double duration);

#endif //MAP_H