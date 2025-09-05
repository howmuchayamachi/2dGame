/*==============================================================================

   �}�b�v�̕\��[map.cpp]
														 Author : Harada Ren
														 Date   : 2025/07/16
--------------------------------------------------------------------------------

==============================================================================*/
#include "map.h"
#include "texture.h"
#include "sprite.h"
#include <DirectXMath.h>
using namespace DirectX;
#include "direct3d.h"
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <stdlib.h>
#include <time.h>



static int g_ScreenMapCountH = 0;
static int g_ScreenMapCountV = 0;

static int g_MapChipTexId = -1;
static int g_MapChipTexId_float = -1;
static int g_MapChipTexId_plants = -1;
static int g_MapChipTexId_Bg_Ground = -1;
static int g_MapChipTexId_Boss_float = -1;

static XMFLOAT2 g_WorldOffset{ 0.0f,0.0f };
//world�̓Q�[�����E�̍��W
//local�͉�ʂ̍��W

static int g_MapWidth = 0;
static int g_MapHeight = 0;

static std::vector<uint32_t> g_Map_Ground;
static std::vector<uint32_t> g_Map_Float_Ground;
static std::vector<uint32_t> g_Map_Wall;
static std::vector<uint32_t> g_Map_Collision;
static std::vector<uint32_t> g_Map_Oneway;
static std::vector<uint32_t> g_Map_Oneway_Collision;
static std::vector<uint32_t> g_Map_Bg_Ground;
static std::vector<uint32_t> g_Map_Bg_Decoration;
static std::vector<uint32_t> g_Map_Boss_Oneway;


constexpr uint32_t FLIPPED_HORIZONTALLY_FLAG = 0x80000000;
constexpr uint32_t FLIPPED_VERTICALLY_FLAG = 0x40000000;
constexpr uint32_t FLIPPED_DIAGONALLY_FLAG = 0x20000000;


static double g_ShakeTimer = 0.0; //�h�ꎞ��
static float g_ShakeFluctuation = 0.0f; //�h�ꕝ

struct TileData {
	int tileId;
	bool flipH;
	bool flipV;
	bool flipD;
};


// CSV�t�@�C����ǂݍ���ŁAg_Map�Ƀf�[�^���i�[����
std::vector<uint32_t> LoadMapFromCSV(const char* filename,int* width, int* height) {
	std::vector<uint32_t> map_data;

	*width = 0;
	*height = 0;

	std::ifstream file(filename);
	if (!file.is_open()) {
		// �t�@�C�����J���Ȃ�������G���[
		return map_data;
	}

	std::string line;
	while (std::getline(file, line)) {
		if (line.empty() || line=="\r") continue;

		//�s�����J�E���g
		(*height)++;
		int current_width = 0;

		std::stringstream ss(line);
		std::string cell;
		while (std::getline(ss, cell, ',')) {
			if (!cell.empty()) {
				uint32_t cell_value = static_cast<uint32_t>(std::stoul(cell));
				if (static_cast<int>(std::stoi(cell)) == -1) {
					map_data.push_back(0);
				}
				else {
					map_data.push_back(cell_value);
				}


				//�񐔂��J�E���g
				current_width++;
			}
		}

		if (*width == 0) {
			*width = current_width;
		}
	}
	file.close();
	return map_data;

}

void Map_Initialize(){
	g_MapChipTexId = Texture_Load(L"resource/map/MossyTileset/MossyTileSet.png");
	g_MapChipTexId_float = Texture_Load(L"resource/map/MossyTileset/MossyFloatingPlatforms.png");
	g_MapChipTexId_plants= Texture_Load(L"resource/map/MossyTileset/MossyHangingPlants.png");
	g_MapChipTexId_Bg_Ground= Texture_Load(L"resource/map/MossyTileset/MossyBackgroundDecoration.png");
	g_MapChipTexId_Boss_float = Texture_Load(L"resource/map/MossyTileset/woodplatform.png");

	g_ScreenMapCountH = Direct3D_GetBackBufferWidth() / MAPCHIP_WIDTH + 2;
	g_ScreenMapCountV = Direct3D_GetBackBufferHeight() / MAPCHIP_HEIGHT + 2;

	g_Map_Ground.clear(); // �Â��f�[�^���N���A
	g_Map_Float_Ground.clear();
	g_Map_Wall.clear();
	g_Map_Collision.clear();
	g_Map_Oneway.clear();
	g_Map_Oneway_Collision.clear();
	g_Map_Bg_Ground.clear();
	g_Map_Bg_Decoration.clear();
	g_Map_Boss_Oneway.clear();

	g_Map_Ground = LoadMapFromCSV("resource/map/useTileSet/map_ground.csv", &g_MapWidth, &g_MapHeight);
	// �_�~�[�̃��[�J���ϐ��ɓǂݍ��܂��Ă��̌��ʂ��̂Ă�
	int dummy_width, dummy_height;
	g_Map_Float_Ground = LoadMapFromCSV("resource/map/useTileSet/map_float_ground.csv", &dummy_width, &dummy_height);
	g_Map_Wall = LoadMapFromCSV("resource/map/useTileSet/map_wall.csv", &dummy_width, &dummy_height);
	g_Map_Collision = LoadMapFromCSV("resource/map/useTileSet/map_collision.csv", &dummy_width, &dummy_height);
	g_Map_Oneway= LoadMapFromCSV("resource/map/useTileSet/map_oneway_platform.csv", &dummy_width, &dummy_height);
	g_Map_Oneway_Collision = LoadMapFromCSV("resource/map/useTileSet/map_oneway_collision.csv", &dummy_width, &dummy_height);
	g_Map_Bg_Ground = LoadMapFromCSV("resource/map/useTileSet/map_bg_ground.csv", &dummy_width, &dummy_height);
	g_Map_Bg_Decoration = LoadMapFromCSV("resource/map/useTileSet/map_bg_decoration.csv", &dummy_width, &dummy_height);
	g_Map_Boss_Oneway = LoadMapFromCSV("resource/map/useTileSet/map_boss_oneway_platform.csv", &dummy_width, &dummy_height);
}

void Map_Finalize(){
	g_Map_Ground.clear();
	g_Map_Float_Ground.clear();
	g_Map_Wall.clear();
	g_Map_Collision.clear();
	g_Map_Oneway.clear();
	g_Map_Oneway_Collision.clear();
	g_Map_Bg_Ground.clear();
	g_Map_Bg_Decoration.clear();
	g_Map_Boss_Oneway.clear();
}

void DrawLayer(int tex_id, const std::vector<uint32_t>& map_layer, int mx, int my, const XMFLOAT2& local_offset,const XMFLOAT4& color){
	int tileset_image_width = Texture_Width(tex_id);
	if (tileset_image_width == 0) return;
	int tileset_width_in_tiles = tileset_image_width / MAPCHIP_WIDTH;

	for (int y = 0; y < g_ScreenMapCountV; y++) {
		for (int x = 0; x < g_ScreenMapCountH; x++) {
			int omx = mx + x;
			int omy = my + y;

			if (omx < 0 || omx >= g_MapWidth || omy < 0 || omy >= g_MapHeight) {
				continue;
			}
			const int index = omy * g_MapWidth + omx;
			if (index < 0 || index >= map_layer.size() || map_layer[index] <= 0) {
				continue;
			}
			mapChipDraw(tex_id,map_layer[index], x, y, local_offset,tileset_width_in_tiles,color);
		}
	}
}

void Map_Draw(){
	//���[���h���W����}�b�v�`�b�v�̃C���f�b�N�X�ɕϊ�
	int mx = Map_GetWorldToMapX(g_WorldOffset.x);
	int my = Map_GetWorldToMapY(g_WorldOffset.y);

	//���[���h���W�̏��������i�}�b�v�`�b�v���łǂꂾ���Y���Ă��邩�j�̌v�Z���C��
	int ox = ((int)g_WorldOffset.x % MAPCHIP_WIDTH + MAPCHIP_WIDTH) % MAPCHIP_WIDTH;
	int oy = ((int)g_WorldOffset.y % MAPCHIP_HEIGHT + MAPCHIP_HEIGHT) % MAPCHIP_HEIGHT;


	float local_offset_x = (float)ox * -1;
	float local_offset_y = (float)oy * -1;

	// �e���C���[���Ƃɕ`��
	DrawLayer(g_MapChipTexId_plants, g_Map_Bg_Decoration, mx, my, { local_offset_x, local_offset_y },{0.0f,1.0f,1.0f,1.0f});
	DrawLayer(g_MapChipTexId_plants, g_Map_Oneway, mx, my, { local_offset_x, local_offset_y });
	DrawLayer(g_MapChipTexId, g_Map_Ground, mx, my, { local_offset_x, local_offset_y });
	DrawLayer(g_MapChipTexId, g_Map_Wall, mx, my, { local_offset_x, local_offset_y });
	DrawLayer(g_MapChipTexId_float, g_Map_Float_Ground, mx, my, { local_offset_x, local_offset_y });
	DrawLayer(g_MapChipTexId_Boss_float, g_Map_Boss_Oneway, mx, my, { local_offset_x, local_offset_y });
}

void Map_SetWorldOffset(const DirectX::XMFLOAT2& world_offset)
{
	g_WorldOffset = world_offset;
}

DirectX::XMFLOAT2 Map_GetWorldOffset() {
	return g_WorldOffset;
}



bool Map_hitJudgementBoxVSMap(const Box& collision) {

	// �����蔻��Ɏg�����W���i�[����z��
	XMFLOAT2 check_points[8];

	float left = collision.center.x - collision.half_width;
	float right = collision.center.x + collision.half_width;
	float top = collision.center.y - collision.half_height;
	float bottom = collision.center.y + collision.half_height;
	float mid_x = collision.center.x;
	float mid_y = collision.center.y;

	//8�_�œ����蔻����m�F
	check_points[0] = { left,  top };    // ����
	check_points[1] = { right, top };    // �E��
	check_points[2] = { left,  bottom }; // ����
	check_points[3] = { right, bottom }; // �E��
	check_points[4] = { mid_x, top };    // ��Ӓ���
	check_points[5] = { mid_x, bottom }; // ���Ӓ���
	check_points[6] = { left,  mid_y };  // ���Ӓ���
	check_points[7] = { right, mid_y };  // �E�Ӓ���


	for (int i = 0; i < 8; i++) {
		int mx = Map_GetWorldToMapX(check_points[i].x);
		int my = Map_GetWorldToMapY(check_points[i].y);

		if (Map_GetMapChipFromLayer(g_Map_Collision, mx, my) > 0) {
			return true; // ������
		}
	}

	// �ǂ̓_����Q���ɓ������Ă��Ȃ�����
	return false;
}

bool Map_hitJudgementCircleVSMap(const Circle& collision){
	// �~���ǂ͈̔͂̃^�C���ɏd�Ȃ��Ă���\�������邩�A��܂��Ɍv�Z����
	int start_mx = Map_GetWorldToMapX(collision.center.x - collision.radius);
	int end_mx = Map_GetWorldToMapX(collision.center.x + collision.radius);
	int start_my = Map_GetWorldToMapY(collision.center.y - collision.radius);
	int end_my = Map_GetWorldToMapY(collision.center.y + collision.radius);

	// ���͈̔͂̃^�C�������ׂă��[�v�Ń`�F�b�N
	for (int my = start_my; my <= end_my; ++my) {
		for (int mx = start_mx; mx <= end_mx; ++mx) {

			// ���̃^�C���������蔻����������ׂ�
			if (Map_GetMapChipFromLayer(g_Map_Ground, mx, my) > 0 ||
				Map_GetMapChipFromLayer(g_Map_Float_Ground, mx, my) > 0 ||
				Map_GetMapChipFromLayer(g_Map_Wall, mx, my) > 0)
			{
				// �����蔻������^�C���Ȃ�A���̃^�C����Box���쐬����
				Box tile_box;
				tile_box.half_width = MAPCHIP_WIDTH / 2.0f;
				tile_box.half_height = MAPCHIP_HEIGHT / 2.0f;
				tile_box.center.x = (float)mx * MAPCHIP_WIDTH + tile_box.half_width;
				tile_box.center.y = (float)my * MAPCHIP_HEIGHT + tile_box.half_height;

				// Box��Circle�̐��m�ȓ����蔻����s��
				if (Collision_IsOverlapCircleVSBox(tile_box, collision)) {
					return true; // 1�ł��������Ă�����A������true��Ԃ�
				}
			}
		}
	}

	// �ǂ̃^�C���Ƃ��������Ă��Ȃ�����
	return false;
}

bool Map_hitJudgementBoxVSOneway(const Box& collision)
{
	// �����蔻��Ɏg�����W���i�[����z��
	XMFLOAT2 check_points[8];

	float left = collision.center.x - collision.half_width;
	float right = collision.center.x + collision.half_width;
	float top = collision.center.y - collision.half_height;
	float bottom = collision.center.y + collision.half_height;
	float mid_x = collision.center.x;
	float mid_y = collision.center.y;

	//8�_�œ����蔻����m�F
	check_points[0] = { left,  top };    // ����
	check_points[1] = { right, top };    // �E��
	check_points[2] = { left,  bottom }; // ����
	check_points[3] = { right, bottom }; // �E��
	check_points[4] = { mid_x, top };    // ��Ӓ���
	check_points[5] = { mid_x, bottom }; // ���Ӓ���
	check_points[6] = { left,  mid_y };  // ���Ӓ���
	check_points[7] = { right, mid_y };  // �E�Ӓ���

	
	for (int i = 0; i < 8; i++) {
		int mx = Map_GetWorldToMapX(check_points[i].x);
		int my = Map_GetWorldToMapY(check_points[i].y);

		if (Map_GetMapChipFromLayer(g_Map_Oneway_Collision, mx, my) > 0) {
			return true; // ������
		}
	}
	

	return false;
}

int Map_GetWorldToMapX(float x){
	//�I�t�Z�b�g����}�b�v���W�̎Z�o
	return (int)(x / MAPCHIP_WIDTH) + (x < 0 ? -1 : 0);
}

int Map_GetWorldToMapY(float y){
	return (int)(y / MAPCHIP_HEIGHT) + (y < 0 ? -1 : 0);
}


int Map_GetMapChipFromLayer(const std::vector<uint32_t>& map_layer, int mx, int my){
	if (mx < 0 || mx >= g_MapWidth || my < 0 || my >= g_MapHeight) {
		return 0;
	}
	const int index = my * g_MapWidth + mx;
	if (index < 0 || index >= map_layer.size()) {
		return 0;
	}
	return map_layer[index];
}


void mapChipDraw(int tex_id, uint32_t mapChipId, int mx, int my, const XMFLOAT2& local_offset, int tileset_width_in_tiles, const DirectX::XMFLOAT4& color) {

	bool is_flipped_horizontally = (mapChipId & FLIPPED_HORIZONTALLY_FLAG);

	bool is_flipped_vertically = (mapChipId & FLIPPED_VERTICALLY_FLAG);

	mapChipId= mapChipId & ~(FLIPPED_HORIZONTALLY_FLAG | FLIPPED_VERTICALLY_FLAG | FLIPPED_DIAGONALLY_FLAG);

	if (mapChipId == 0) return;


	float px = (float)(mx * MAPCHIP_WIDTH) + local_offset.x;
	float py = (float)(my * MAPCHIP_HEIGHT) + local_offset.y;

	if (tileset_width_in_tiles == 0) return;

	// �P������ID���Q�����̍��W�i�^�C���Z�b�g�̉���ځA���s�ڂ��j�ɕϊ�����
	int tile_col = mapChipId % tileset_width_in_tiles; // �� (X)
	int tile_row = mapChipId / tileset_width_in_tiles; // �s (Y)

	// �؂�o���e�N�X�`���̍���̃s�N�Z�����W���ŏI�I�Ɍv�Z
	int cut_x = tile_col * MAPCHIP_WIDTH;
	int cut_y = tile_row * MAPCHIP_HEIGHT;

	Sprite_Draw(tex_id, px, py,
		MAPCHIP_WIDTH, MAPCHIP_HEIGHT,  // ��ʂɕ`�悷��T�C�Y
		cut_x, cut_y,                  // �e�N�X�`������؂�o�����W
		MAPCHIP_WIDTH, MAPCHIP_HEIGHT,  // �e�N�X�`������؂�o���T�C�Y
		is_flipped_horizontally,
		is_flipped_vertically,
		color
	);
}


void Map_UpdateCamera(const DirectX::XMFLOAT2& player_pos, double elapsed_time) {
	float screen_width = (float)Direct3D_GetBackBufferWidth();
	float margin_left = screen_width * 0.50f - 64.0f;
	float margin_right = screen_width * 0.50f - 64.0f;

	float screen_height = (float)Direct3D_GetBackBufferHeight();
	float margin_top = screen_height * 0.50f - 64.0f;
	float margin_bottom = screen_height * 0.70f - 64.0f;

	// �J�����̖ڕW�ʒu�����݈ʒu�ŏ�����
	XMFLOAT2 target_offset = g_WorldOffset;

	// X����
	float player_screen_x = player_pos.x - g_WorldOffset.x;
	if (player_screen_x < margin_left) {
		target_offset.x = player_pos.x - margin_left;
	}
	else if (player_screen_x > margin_right) {
		target_offset.x = player_pos.x - margin_right;
	}

	// Y����
	float player_screen_y = player_pos.y - g_WorldOffset.y;
	if (player_screen_y < margin_top) {
		target_offset.y = player_pos.y - margin_top;
	}
	else if (player_screen_y > margin_bottom) {
		target_offset.y = player_pos.y - margin_bottom;
	}

	// �X���[�Y�ɒǏ]�i�C�[�W���O�j
	const float CAMERA_SMOOTHING = 0.2f;
	g_WorldOffset.x += (target_offset.x - g_WorldOffset.x) * CAMERA_SMOOTHING;
	g_WorldOffset.y += (target_offset.y - g_WorldOffset.y) * CAMERA_SMOOTHING;


	//�J������h�炷
	if (g_ShakeTimer > 0.0) {
		g_ShakeTimer -= elapsed_time;

		//�����_���ȗh��𐶐�
		float shakeX = ((float)rand() / RAND_MAX * 2.0f - 1.0f) * g_ShakeFluctuation;
		float shakeY = ((float)rand() / RAND_MAX * 2.0f - 1.0f) * g_ShakeFluctuation;

		//���̃J�������W�ɉ��Z
		g_WorldOffset.x += shakeX;
		g_WorldOffset.y += shakeY;

		if (g_ShakeTimer <= 0.0) {
			g_ShakeTimer = 0.0;
			g_ShakeFluctuation = 0.0f;
		}
	}

	// �J�������}�b�v�O�ɍs���Ȃ��悤����
	if (g_WorldOffset.x < 0.0f) g_WorldOffset.x = 0.0f;
	if (g_WorldOffset.y < 0.0f) g_WorldOffset.y = 0.0f;

	float map_pixel_width = (float)(g_MapWidth * MAPCHIP_WIDTH);
	float map_pixel_height = (float)(g_MapHeight * MAPCHIP_HEIGHT);

	if (g_WorldOffset.x > map_pixel_width - screen_width) {
		g_WorldOffset.x = map_pixel_width - screen_width;
	}

	if (g_WorldOffset.y > map_pixel_height - screen_height) {
		g_WorldOffset.y = map_pixel_height - screen_height;
	}
}

void Map_StartShake(float fluctuation, double duration){
	g_ShakeFluctuation = fluctuation;
	g_ShakeTimer = duration;
}
