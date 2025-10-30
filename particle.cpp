/*==============================================================================

  �p�[�e�B�N���`��[particle.cpp]
														 Author : Harada Ren
														 Date   : 2025/09/03
--------------------------------------------------------------------------------

==============================================================================*/
#include "particle.h"
#include "texture.h"
#include "sprite.h"
#include "direct3d.h"
using namespace DirectX;
#include "map.h"
#include <stdlib.h>
#include <time.h>
#include "Player.h"


struct Particle {
	ParticleType type;
	XMFLOAT2 position;
	XMFLOAT2 velocity;
	XMFLOAT4 color;
	float size;
	double lifeTime;
	double birthTime; //�O��������g���Ă��Ȃ�
	float randomPhase; //�����_���Ȕg�̈ʑ�
	XMFLOAT2 targetPosition;
};

static constexpr unsigned int PARTICLE_MAX = 50000;
static Particle g_Paticles[PARTICLE_MAX]{};
static int g_ParticleTexId = -1;
static double g_Time = 0.0;

void Particle_Initialize() {
	for (Particle& p : g_Paticles) {
		p.birthTime = 0.0;
	}
	g_Time = 0.0;

	g_ParticleTexId = Texture_Load(L"resource/texture/effect000.jpg");

	srand((unsigned int)time(NULL));
}

void Particle_Finalize() {
}

void Particle_Update(double elapsed_time) {
	for (Particle& p : g_Paticles) {
		if (p.birthTime == 0.0) continue;

		double time = g_Time - p.birthTime;
		if (time > p.lifeTime) {
			p.birthTime = 0.0; //�������s����
			continue;
		}

		switch (p.type) {
		case PLAYER_BULLET:
			break;

		case ENEMY_BULLET:
			break;

		case CHARGE:
			p.position.x += ((float)rand() / RAND_MAX - 0.5f) * 10.0f;
			p.position.y += ((float)rand() / RAND_MAX - 0.5f) * 10.0f;
			break;

		case GATHERING_POWER: {
			// ���݈ʒu����ڕW�n�_�ւ̕����x�N�g�����v�Z
			XMVECTOR current_pos = XMLoadFloat2(&p.position);
			XMVECTOR target_pos = XMLoadFloat2(&p.targetPosition);
			XMVECTOR direction = XMVector2Normalize(target_pos - current_pos);

			// �ڕW�Ɍ������͂𑬓x�ɉ��Z����i���񂾂񑬂��Ȃ�j
			float acceleration = 2000.0f; // �����񂹂�͂̋���
			XMVECTOR velocity = XMLoadFloat2(&p.velocity);
			velocity += direction * acceleration * (float)elapsed_time;
			XMStoreFloat2(&p.velocity, velocity);

			// ��C��R�ŁA�ō����x��}����
			p.velocity.x *= 0.99f;
			p.velocity.y *= 0.99f;

		}
			break;

		case KINGS_DROP:
			break;

		case KINGS_EXPLOSION:
			p.velocity.x *= 0.99f;
			p.velocity.y *= 0.99f;
			break;

		case PLAYER_DEATH:
			p.velocity.y *= 0.99f; // ��C��R
			p.velocity.x += sinf((float)g_Time * 5.0f + p.randomPhase) * 2.0f; // ����炳����
			break;

		case ENEMY_DEATH:
			p.velocity.x *= 0.99f;
			p.velocity.y *= 0.99f;
			break;

		case BOSS_DEATH:
			p.velocity.x *= 0.99f;
			p.velocity.y *= 0.99f;
			break;

		default:
			break;
		}

		p.position.x += p.velocity.x * (float)elapsed_time;
		p.position.y += p.velocity.y * (float)elapsed_time;
	}

	//���ԍX�V
	g_Time += elapsed_time;
}

void Particle_Draw() {
	Direct3D_SetAlphaBlendAdd();

	XMFLOAT2 offset = Map_GetWorldOffset();

	for (const Particle& p : g_Paticles) {
		if (p.birthTime == 0.0) continue;

		float screen_x = p.position.x - offset.x;
		float screen_y = p.position.y - offset.y;
		//���܂Ő���������
		double time = g_Time - p.birthTime;
		//�����̓�����������
		float time_ratio = (float)(time / p.lifeTime);
		float size = p.size * (1.0f - time_ratio);
		float half_size = size * 0.5f;
		XMFLOAT4 color = p.color;
		color.w = p.color.w * (1.0f - time_ratio);

		Sprite_Draw(g_ParticleTexId, screen_x + half_size + p.size * time_ratio, screen_y + half_size + p.size * time_ratio, size, size, color);
	}

	Direct3D_SetAlphaBlendTransparent();
}

void Particle_Create(ParticleType type, const DirectX::XMFLOAT2& position) {
	int create_count = 1;
	switch (type) {
	case PLAYER_BULLET:
		//�v���C���[�̒e�̋O��
		break;

	case ENEMY_BULLET:
		//�G�l�~�[�̒e�̋O��
		break;

	case CHARGE:
		//���U�����̃I�[��
		break;

	case GATHERING_POWER:
		create_count = 50;
		break;

	case KINGS_DROP:
		break;

	case KINGS_EXPLOSION:
		create_count = 100;
		break;

	case PLAYER_DEATH:
		create_count = 30;
		break;

	case ENEMY_DEATH:
		create_count = 20;
		break;

	case BOSS_DEATH:
		create_count = 100;
		break;

	default:
		break;
	}

	for (int i = 1;i <= create_count;i++) {

		XMFLOAT3 Particle_color{};

		for (Particle& p : g_Paticles) {
			//�󂫗̈��T��
			if (p.birthTime != 0.0) continue;

			p.type = type;
			p.birthTime = g_Time;
			p.position = position;

			switch (type) {
			case PLAYER_BULLET:
				p.lifeTime = 0.5;
				p.size = 64.0f;
				p.velocity = { 0.0f,0.0f };
				p.color = { 1.0f,0.5f,0.5f,1.0f };
				break;

			case ENEMY_BULLET:
				p.lifeTime = 0.5;
				p.size = 32.0f;
				p.velocity = { 0.0f,0.0f };
				p.color = { 0.5f,0.5f,1.0f,1.0f };
				break;

			case CHARGE:
				p.lifeTime = 0.3f;
				p.size = 32.0f;
				p.velocity = { 0.0f,0.0f };

				Particle_color.x = ((float)rand() / RAND_MAX * 0.9f) + 0.1f;
				Particle_color.y = ((float)rand() / RAND_MAX * 0.9f) + 0.1f;
				Particle_color.z = ((float)rand() / RAND_MAX * 0.9f) + 0.1f;
				p.color = { Particle_color.x,Particle_color.y,Particle_color.z,1.0f };

				p.position.x += -32.0f + ((float)rand() / RAND_MAX - 0.5f) * 100.0f;
				p.position.y -= 32.0f + ((float)rand() / RAND_MAX) * 300.0f;

				break;

			case GATHERING_POWER: {
				p.lifeTime = 0.8f;
				p.size = 32.0f;
				p.velocity = { 0.0f,0.0f };
				p.color = { 0.5f,0.5f,1.0f,1.0f };

				p.targetPosition = position;

				float angle = ((float)rand() / RAND_MAX) * 2.0f * XM_PI;
				float distance = 250.0f + ((float)rand() / RAND_MAX) * 100.0f;

				p.position.x = p.targetPosition.x + cosf(angle) * distance;
				p.position.y = p.targetPosition.y + sinf(angle) * distance;
			}
				break;

			case KINGS_DROP:
				p.lifeTime = 3.0f;
				p.size = 64.0f;
				p.velocity = { 0.0f,0.0f };
				p.color = { 0.5f,0.5f,1.0f,1.0f };

				p.targetPosition = Player_GetPosition();
				break;

			case KINGS_EXPLOSION: {
				p.lifeTime = 1.0f;
				p.size = 50.0f;
				p.velocity = { 0.0f,0.0f };

				// 0�x����360�x�̃����_���Ȋp�x���v�Z
				float angle = ((float)rand() / RAND_MAX) * 2.0f * XM_PI;

				// �����_���ȃX�s�[�h���v�Z
				float speed = 1300.0f + ((float)rand() / RAND_MAX) * 200.0f;

				// �p�x�ƃX�s�[�h����AX������Y�����̏���������
				p.velocity.x = cosf(angle) * speed;
				p.velocity.y = sinf(angle) * speed;

				if (rand() % 3 == 0) {
					Particle_color.x = 1.0f;
					Particle_color.z = 0.1f;
				}
				else {
					Particle_color.x = 0.1f;
					Particle_color.z = 1.0f;
				}
				Particle_color.y = 0.1f;

				p.color = { Particle_color.x,Particle_color.y,Particle_color.z,1.0f };
			}
				break;

			case PLAYER_DEATH:
				p.lifeTime = 1.0f;
				p.size = 32.0f;
				//-50~50
				p.velocity = { ((float)rand() / RAND_MAX - 0.5f) * 100.0f, -100.0f };
				p.color = { 1.0f,0.1f,0.1f,1.0f };
				p.position.x += ((float)rand() / RAND_MAX - 0.5f) * 60.0f;
				p.position.y -= ((float)rand() / RAND_MAX) * 100.0f;

				p.randomPhase = ((float)rand() / RAND_MAX) * 10.0f;
				break;

			case ENEMY_DEATH: {
				p.lifeTime = 1.5f;
				p.size = 16.0f;

				// 0�x����360�x�̃����_���Ȋp�x���v�Z (���W�A���P��)
				float angle = ((float)rand() / RAND_MAX) * 2.0f * XM_PI;

				// 50����250�̊Ԃ̃����_���ȃX�s�[�h���v�Z
				float speed = 50.0f + ((float)rand() / RAND_MAX) * 200.0f;

				// �p�x�ƃX�s�[�h����AX������Y�����̏���������
				p.velocity.x = cosf(angle) * speed;
				p.velocity.y = sinf(angle) * speed;

				p.color = { 0.5f,0.5f,1.0f,1.0f };
			}
				break;

			case BOSS_DEATH: {
				p.lifeTime = 2.0f;
				p.size = 32.0f;

				// 0�x����360�x�̃����_���Ȋp�x���v�Z (���W�A���P��)
				float angle = ((float)rand() / RAND_MAX) * 2.0f * XM_PI;

				// 400����600�̊Ԃ̃����_���ȃX�s�[�h���v�Z
				float speed = 400.0f + ((float)rand() / RAND_MAX) * 200.0f;

				// �p�x�ƃX�s�[�h����AX������Y�����̏���������
				p.velocity.x = cosf(angle) * speed;
				p.velocity.y = sinf(angle) * speed;

				Particle_color.x = (float)(rand() % 10 + 1) / 10.0f;
				Particle_color.y = (float)(rand() % 10 + 1) / 10.0f;
				Particle_color.z = (float)(rand() % 10 + 1) / 10.0f;
				p.color = { Particle_color.x,Particle_color.y,Particle_color.z,1.0f };
			}
				break;

			default:
				break;
			}

			break;
		}
	}
}
