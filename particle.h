/*==============================================================================

  �p�[�e�B�N���`��[particle.h]
														 Author : Harada Ren
														 Date   : 2025/09/03
--------------------------------------------------------------------------------

==============================================================================*/
#ifndef PARTICLE_H
#define PARTICLE_H

#include <DirectXMath.h>

enum ParticleType {
	PLAYER_BULLET,
	ENEMY_BULLET,
	CHARGE, //���`���[�W��
	GATHERING_POWER, //�{�X�K�E�Z�O�̃`���[�W
	KINGS_DROP, //�{�X�K�E�Z����
	KINGS_EXPLOSION, //�{�X�K�E�Z����
	PLAYER_DEATH,
	ENEMY_DEATH,
	BOSS_DEATH,
	MAX
};

void Particle_Initialize();
void Particle_Finalize();
void Particle_Update(double elapsed_time);
void Particle_Draw();

void Particle_Create(ParticleType type, const DirectX::XMFLOAT2& position);


#endif //PARTICLE_H