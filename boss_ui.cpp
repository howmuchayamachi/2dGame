/*====================================================================================

�{�XUI�Ǘ�[boss_ui.h]

																Author	: Harada Ren
																Date	: 2025/08/13
--------------------------------------------------------------------------------------

======================================================================================*/
#include "boss_ui.h"
#include "texture.h"
#include "sprite.h"
#include "boss.h"
#include "direct3d.h"
#include <DirectXMath.h>
using namespace DirectX;
#include "Audio.h"


static int g_BossNameTexId = -1;
static int g_BossHpBarTexId = -1;
static int g_BossHpRedTexId = -1;
static int g_BossHpFrameTexId = -1;

static int g_BossGageAudioId = -1;

static float g_BossHpRedValue = 0.0f;

//�{�X��J�n��HP�o�[�\���p�ϐ�
static float g_BossHpAnimValue = 0.0f;
static bool g_IsHpBarAnim = false;


void BossUI_Initialize() {
	g_BossNameTexId = Texture_Load(L"resource/texture/Font/boss_name.png");
	g_BossHpBarTexId = Texture_Load(L"resource/texture/ui/boss_hp.png");
	g_BossHpRedTexId = Texture_Load(L"resource/texture/ui/hp_red.png");
	g_BossHpFrameTexId = Texture_Load(L"resource/texture/ui/hp_frame.png");

	g_BossGageAudioId = LoadAudio("resource/audio/boss_gage.wav");

	g_BossHpRedValue = (float)BOSS_MAXHP;

	g_BossHpAnimValue = 0.0f;
	g_IsHpBarAnim = false;
}

void BossUI_Finalize() {
}

void BossUI_Update(double elapsed_time) {
	if (!Boss_IsAlive()) return;

	if (g_IsHpBarAnim) {
		g_BossHpAnimValue += 30.0f * (float)elapsed_time;
		//�ő�HP�ɂȂ�����A�j���[�V�����I��
		if (g_BossHpAnimValue >= (float)BOSS_MAXHP) {
			g_BossHpAnimValue = (float)BOSS_MAXHP;
			g_IsHpBarAnim = false;
			StopAudio(g_BossGageAudioId);
		}
	}

	//�v���C���[�̌��݂�HP���擾
	float current_hp = (float)Boss_GetHp();

	//�Ԃ�HP�o�[���{����HP�����傫���ꍇ
	if (g_BossHpRedValue > current_hp) {
		//�������Ԃ�HP�o�[�����炷
		g_BossHpRedValue -= 2.0f * (float)elapsed_time;
		if (g_BossHpRedValue < current_hp) {
			g_BossHpRedValue = current_hp;
		}
	}
}

void BossUI_Draw() {
	if (!Boss_IsAlive()) return;

	float screen_width = (float)Direct3D_GetBackBufferWidth();
	float screen_height = (float)Direct3D_GetBackBufferHeight();
	// �E���HP�o�[�̘g��\��
	Sprite_Draw(g_BossHpFrameTexId, screen_width - 1480.0f, screen_height - 120.0f, 1600.0f, 100.0f);

	//�o�ꎞ���o���ǂ���
	float hp_disp = g_IsHpBarAnim ? g_BossHpAnimValue : g_BossHpRedValue;
	float damage_bar_ratio = hp_disp / (float)(BOSS_MAXHP);
	Sprite_Draw(g_BossHpRedTexId, screen_width - 1150.0f, screen_height - 110.0f, 1100.0f * damage_bar_ratio, 80.0f);

	// ���݂�HP�̊������v�Z (0.0�`1.0)
	hp_disp = g_IsHpBarAnim ? g_BossHpAnimValue : (float)Boss_GetHp();
	float hp_ratio = hp_disp / (float)BOSS_MAXHP;

	// HP�̊����ɉ����āA���g�̃o�[�̕`�悷�镝��ς���
	Sprite_Draw(g_BossHpBarTexId, screen_width - 1150.0f, screen_height - 110.0f, 1100.0f * hp_ratio, 80.0f);

	//�{�X���\��
	Sprite_Draw(g_BossNameTexId, screen_width - 1460.0f, screen_height - 110.0f, 300.0f, 75.0f);
}

void BossUI_StartAnim() {
	g_BossHpAnimValue = 0.0f;
	g_IsHpBarAnim = true;
	PlayAudio(g_BossGageAudioId, false, 0.5f);
}
