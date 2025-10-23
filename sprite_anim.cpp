/*====================================================================================

�X�v���C�g�A�j���[�V�����`��[sprite_anim.h]

																Author	: Harada Ren
																Date	: 2025/06/17
--------------------------------------------------------------------------------------

======================================================================================*/
#include "sprite_anim.h"
#include "sprite.h"
#include "texture.h"
#include <DirectXMath.h>
using namespace DirectX;

//�A�j���[�V�����p�^�[��
struct AnimPatternData {
	int m_TextureId = -1; //�e�N�X�`��ID
	int m_PatternMax = 0; //�p�^�[����
	int m_HPatternMax = 0; //���̃p�^�[���ő吔
	XMUINT2 m_StartPosition{ 0,0 }; //�A�j���[�V�����̃X�^�[�g���W
	XMUINT2 m_PatternSize{ 0,0 }; //1�p�^�[���̕��ƍ���
	double m_seconds_per_pattern = 0.1; //�p�^�[��������b�\�����邩
	bool m_IsLooped = true; //���[�v���邩(���������邩)
};

//�Đ���
struct AnimPlayData {
	int m_PatternId = -1; //�A�j���[�V�����p�^�[��ID
	int m_PatternNum = 0; //���ݍĐ����̃p�^�[���ԍ�
	double m_accumulated_time = 0.0; //�ݐώ���
	bool m_IsStopped = false; //�A�j���[�V�����̍Ō�܂ōs������
};


static constexpr int ANIM_PATTERN_MAX = 1024; //�p�^�[���̍ő�l
static AnimPatternData g_AnimPattern[ANIM_PATTERN_MAX];
static constexpr int ANIM_PLAY_MAX = 512; //�A�j���[�V�����̍Đ��𓯎��ɉ����邩
static AnimPlayData g_AnimPlay[ANIM_PLAY_MAX];



void SpriteAnim_Initialize() {
	//�A�j���[�V�����p�^�[���Ǘ�����������
	for (AnimPatternData& data : g_AnimPattern) {
		data.m_TextureId = -1; //�e�N�X�`��id��-1��������g���Ă��Ȃ�����
	}

	for (AnimPlayData& data : g_AnimPlay) {
		data.m_PatternId = -1;
		data.m_IsStopped = false;
	}
}

void SpriteAnim_Finalize() {
	for (int i = 0; i < ANIM_PLAY_MAX; i++) {
		g_AnimPlay[i].m_PatternId = -1;
	}
	for (int i = 0; i < ANIM_PATTERN_MAX; i++) {
		g_AnimPattern[i].m_TextureId = -1;
	}
}

void SpriteAnim_Update(double elapsed_time) {
	for (int i = 0; i < ANIM_PLAY_MAX; i++) {
		if (g_AnimPlay[i].m_PatternId < 0) {
			continue;
		}

		AnimPatternData* pAnimPatternData = &g_AnimPattern[g_AnimPlay[i].m_PatternId];

		if (g_AnimPlay[i].m_accumulated_time >= pAnimPatternData->m_seconds_per_pattern) {
			g_AnimPlay[i].m_PatternNum++;



			if (g_AnimPlay[i].m_PatternNum >= pAnimPatternData->m_PatternMax) {

				if (pAnimPatternData->m_IsLooped) {
					g_AnimPlay[i].m_PatternNum = 0;
				}
				else {
					g_AnimPlay[i].m_PatternNum = pAnimPatternData->m_PatternMax - 1;
					g_AnimPlay[i].m_IsStopped = true;
				}
			}

			g_AnimPlay[i].m_accumulated_time -= pAnimPatternData->m_seconds_per_pattern;

		}
		g_AnimPlay[i].m_accumulated_time += elapsed_time;
	}
}

void SpriteAnim_Draw(int playid, float dx, float dy, float dw, float dh,bool IsFlipped,const XMFLOAT4 color) {

	int anim_pattern_id = g_AnimPlay[playid].m_PatternId;
	AnimPatternData* pAnimPatternData = &g_AnimPattern[anim_pattern_id];
	Sprite_Draw(pAnimPatternData->m_TextureId,
		dx, dy, dw, dh,
		pAnimPatternData->m_StartPosition.x
		+ pAnimPatternData->m_PatternSize.x
		* (g_AnimPlay[playid].m_PatternNum % pAnimPatternData->m_HPatternMax),

		pAnimPatternData->m_StartPosition.y + pAnimPatternData->m_PatternSize.y
		* (g_AnimPlay[playid].m_PatternNum / pAnimPatternData->m_HPatternMax),

		pAnimPatternData->m_PatternSize.x,
		pAnimPatternData->m_PatternSize.y,
		IsFlipped,
		false,
		color
	);

}

int SpriteAnim_RegisterPattern(int texid, int pattern_max, int h_pattern_max, double m_seconds_per_pattern,
	const DirectX::XMUINT2& pattern_size, const DirectX::XMUINT2& start_position, bool is_looped) {
	for (int i = 0; i < ANIM_PATTERN_MAX; i++) {
		//�J���Ă�ꏊ��T��
		if (g_AnimPattern[i].m_TextureId >= 0) continue;

		g_AnimPattern[i].m_TextureId = texid;
		g_AnimPattern[i].m_PatternMax = pattern_max;
		g_AnimPattern[i].m_HPatternMax = h_pattern_max;
		g_AnimPattern[i].m_seconds_per_pattern = m_seconds_per_pattern;
		g_AnimPattern[i].m_PatternSize = pattern_size;
		g_AnimPattern[i].m_StartPosition = start_position;
		g_AnimPattern[i].m_IsLooped = is_looped;
		return i;
	}
	return -1;
}

int SpriteAnim_CreatePlayer(int anim_pattern_id) {
	for (int i = 0; i < ANIM_PLAY_MAX; i++) {
		if (g_AnimPlay[i].m_PatternId >= 0) continue;

		g_AnimPlay[i].m_PatternId = anim_pattern_id;
		g_AnimPlay[i].m_accumulated_time = 0.0;
		g_AnimPlay[i].m_PatternNum = 0;

		g_AnimPlay[i].m_IsStopped = false;
		return i;
	}
	return -1;
}

bool SpriteAnim_IsStopped(int index){
	return g_AnimPlay[index].m_IsStopped;
}

void SpriteAnim_DestroyPlayer(int index){
	g_AnimPlay[index].m_PatternId = -1; 
}

int SpriteAnim_GetPatternNum(int playid)
{
	if (playid < 0 || playid >= ANIM_PLAY_MAX || g_AnimPlay[playid].m_PatternId < 0) {
		return -1; // �G���[�̏ꍇ��-1��Ԃ�
	}
	return g_AnimPlay[playid].m_PatternNum;
}
