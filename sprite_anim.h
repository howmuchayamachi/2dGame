/*====================================================================================

スプライトアニメーション描画[sprite_anim.h]

																Author	: Harada Ren
																Date	: 2025/06/17
--------------------------------------------------------------------------------------

======================================================================================*/
#ifndef SPRITE_ANIM_H
#define SPRITE_ANIM_H

#include <DirectXMath.h>


void SpriteAnim_Initialize();
void SpriteAnim_Finalize();

void SpriteAnim_Update(double elapsed_time);
void SpriteAnim_Draw(int playid,float dx, float dy, float dw, float dh, bool IsFlipped = false,const DirectX::XMFLOAT4 color={1.0f,1.0f,1.0f,1.0f});

int SpriteAnim_RegisterPattern(int texid, int pattern_max, int h_pattern_max,
	double m_seconds_per_pattern,
	const DirectX::XMUINT2& pattern_size, const DirectX::XMUINT2& start_position, 
	bool is_looped=true);

int SpriteAnim_CreatePlayer(int anim_pattern_id);

bool SpriteAnim_IsStopped(int index);

void SpriteAnim_DestroyPlayer(int index);

int SpriteAnim_GetPatternNum(int playid);

#endif //SPRITE_ANIM
