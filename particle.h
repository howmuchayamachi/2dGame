/*==============================================================================

  パーティクル描画[particle.h]
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
	CHARGE, //剣チャージ時
	GATHERING_POWER, //ボス必殺技前のチャージ
	KINGS_DROP, //ボス必殺技投下
	KINGS_EXPLOSION, //ボス必殺技爆発
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