/*==============================================================================

   �G�̐��� [enemy.cpp]
														 Author : Harada Ren
														 Date   : 2025/07/02
--------------------------------------------------------------------------------

==============================================================================*/
#include "enemy.h"
#include "DirectXMath.h"
using namespace DirectX;
#include "texture.h"
#include "sprite.h"
#include "sprite_anim.h"
#include "collision.h"
#include "effect.h"
#include "bullet.h"
#include "map.h"
#include "Player.h"
#include "boss.h"
#include <cmath>
#include "direct3d.h"
#include "particle.h"

struct EnemyType {
	int TexId;
	//int tx, ty, tw, th; //�摜��؂���ꍇ�Ɏg��
	//���x�Ȃ�

	Circle collision;
	Box box_collision;
	int hp;//�ő�HP
	unsigned int score; //�|�����Ƃ��̃X�R�A
	int AnimId;
};

enum EnemyState {
	PATROL, //���̓���
	CHASE // �v���C���[��ǐ�
};

struct Enemy {
	int typeId;
	XMFLOAT2 position;
	XMFLOAT2 velocity;
	float offsetY;
	double lifeTime;
	double TimetoFire;
	float hp; //���݂�HP
	float hp_red;
	bool isEnable; //�g���Ă��邩�ǂ���
	bool isDamage; //�_���[�W���󂯂���
	int AnimPlayId;
	EnemyState state;
	bool isFacingRight;
	float InvincibleTime;
	bool isKnockback;
	float KnockbackTimer;
	float ChaseTimer; //�_���[�W���󂯂���Ƀv���C���[��ǂ�����
};


static Enemy g_Enemys[ENEMY_MAX]{};

static int g_EnemyTexId = -1;

static constexpr float ENEMY_WIDTH = 128.0f;
static constexpr float ENEMY_HEIGHT = 128.0f;

static EnemyType g_EnemyTypes[]{ {},{},{} };

static int g_HpBarTexId = -1;
static int g_HpRedTexId = -1;
static int g_HpFrameTexId = -1;

void Enemy_Initialize() {
	for (Enemy& e : g_Enemys) {
		e.isEnable = false;
		e.lifeTime = 0;
		e.TimetoFire = 0.0;
		e.InvincibleTime = 0.0f;
		e.isKnockback = false;
		e.KnockbackTimer = 0.0f;
		e.ChaseTimer = 0.0f;
		SpriteAnim_DestroyPlayer(e.AnimPlayId);
	}

	g_EnemyTypes[0].collision = { {64.0f,64.0f},50.0f };
	g_EnemyTypes[0].TexId = Texture_Load(L"resource/texture/enemy/slime_green_anim.png");
	g_EnemyTypes[0].hp = 1;
	g_EnemyTypes[0].AnimId = SpriteAnim_RegisterPattern(
		g_EnemyTypes[0].TexId, 30, 5, 0.1, { 376,256 }, { 0,0 });

	g_EnemyTypes[1].collision = { {64.0f,64.0f},50.0f };
	g_EnemyTypes[1].TexId = Texture_Load(L"resource/texture/enemy/slime_orange_anim.png");
	g_EnemyTypes[1].hp = 3;
	g_EnemyTypes[1].AnimId = SpriteAnim_RegisterPattern(
		g_EnemyTypes[1].TexId, 30, 5, 0.1, { 510,410 }, { 0,0 });

	g_EnemyTypes[2].collision = { {64.0f,64.0f},50.0f };
	g_EnemyTypes[2].TexId = Texture_Load(L"resource/texture/enemy/bird_anim.png");
	g_EnemyTypes[2].hp = 3;
	g_EnemyTypes[2].AnimId = SpriteAnim_RegisterPattern(
		g_EnemyTypes[2].TexId, 4, 2, 0.1, { 80,80 }, { 0,0 });

	g_HpBarTexId = Texture_Load(L"resource/texture/ui/hp.png");
	g_HpRedTexId = Texture_Load(L"resource/texture/ui/hp_red.png");
	g_HpFrameTexId = Texture_Load(L"resource/texture/ui/hp_frame.png");
}

void Enemy_Finalize() {
	for (Enemy& e : g_Enemys) {
		SpriteAnim_DestroyPlayer(e.AnimPlayId);
	}
}

void Enemy_Update(double elapsed_time) {

	//�v���C���[�̈ʒu���擾
	XMFLOAT2 player_pos_float2 = Player_GetPosition();
	XMVECTOR player_position = XMLoadFloat2(&player_pos_float2);

	for (int i = 0; i < ENEMY_MAX; i++) {
		Enemy& e = g_Enemys[i];
		//�g���Ă��Ȃ�������ǂݔ�΂�
		if (!e.isEnable) continue;
		//��O����
		if (e.typeId < 0 || e.typeId >= ENEMY_TYPE_MAX) return;

		if (e.InvincibleTime > 0.0f) {
			e.InvincibleTime -= (float)elapsed_time;
		}

		if (e.ChaseTimer > 0.0f) {
			e.ChaseTimer -= (float)elapsed_time;
		}

		e.lifeTime += elapsed_time;

		if (e.lifeTime > 15.0) {
			Enemy_Destroy(i);
			e.isEnable = false;
		}

		// ���Z�ł���\���̂ɕϊ�
		XMVECTOR position = XMLoadFloat2(&e.position);
		XMVECTOR velocity = XMLoadFloat2(&e.velocity);

		//�d��
		XMFLOAT2 gdir{ 0.0f, 1.0f };
		float gravity = 9.8f * 200.0f * (float)elapsed_time;


		//�G�l�~�[�X�e�[�g�}�V��
		float move_speed = 100.0f;
		float distance = XMVectorGetX(XMVector2Length(player_position - position));

		if (e.isKnockback) {
			e.KnockbackTimer -= (float)elapsed_time;
			velocity += XMLoadFloat2(&gdir) * gravity;
			if (e.KnockbackTimer <= 0.0f) {
				e.KnockbackTimer = 0.0f;
				e.isKnockback = false;
				e.ChaseTimer = 3.0f;

				//HP0��������G�t�F�N�g���c���ď���
				if (e.hp <= 0) {
					Enemy_Destroy(i);
					continue;
				}
			}
		}
		else {
			switch (e.typeId) {
			case ENEMY_TYPE_SLIME_GREEN:
				velocity += XMLoadFloat2(&gdir) * gravity;
				break;

			case ENEMY_TYPE_SLIME_ORANGE:
				velocity += XMLoadFloat2(&gdir) * gravity;

				move_speed = 200.0f;
				if (std::fmod(e.lifeTime, 4.0) < elapsed_time) {
					float jump_power = -800.0f; // Y�����̃W�����v��
					// Y�����̑��x���W�����v�͂ŏ㏑������BX�����̑��x�͂��̂܂܈ێ�
					velocity = XMVectorSetY(velocity, jump_power);
				}

				if (e.TimetoFire > 3) {
					EnemyBullet_Create({ e.position.x,e.position.y }, Player_GetPosition());
					e.TimetoFire = 0.0;
				}

				break;

			case ENEMY_TYPE_BIRD:
				move_speed = 150.0f;
				break;

			default:
				break;
			}

			//�_���[�W���󂯂�����Ȃ狭���I��CHASE���[�h
			if (e.ChaseTimer > 0.0f) {
				e.state = CHASE;
			}

			switch (e.state) {
			case PATROL:
				velocity = XMVectorSetX(velocity, e.isFacingRight ? move_speed : -move_speed);

				//�v���C���[�Ƃ̋�������苗���ȓ���������CHASE�Ɉڍs
				if (distance < 500.0f) {
					e.state = CHASE;
				}

				break;

			case CHASE:
				e.TimetoFire += elapsed_time;
				move_speed *= 1.5f;
				{			
					//����X������Y��������
					if (e.typeId == ENEMY_TYPE_BIRD) {
						XMVECTOR direction = player_position - position;
						direction = XMVector2Normalize(direction);
						velocity = direction * move_speed;
					}
					//���ȊO��X�����̂�
					else {
						float directionX = (XMVectorGetX(player_position) > XMVectorGetX(position)) ? 1.0f : -1.0f;
						velocity = XMVectorSetX(velocity, move_speed * directionX);
					}
				}

				//�v���C���[�Ƃ̋�������苗�����ꂽ��PATROL�Ɉڍs
				if (distance > 700.0f && e.ChaseTimer<=0.0f) {
					e.state = PATROL;
					e.TimetoFire = 0.0;
				}
				break;

			default:
				break;
			}
		}

		//�����蔻��

		//�ǂƂ̓����蔻��
		XMVECTOR horizontal_move = XMVectorSet(XMVectorGetX(velocity) * (float)elapsed_time, 0.0f, 0.0f, 0.0f);
		position += horizontal_move;
		XMStoreFloat2(&e.position, position);

		if (Map_hitJudgementBoxVSMap(Enemy_GetBoxCollision(i))) {
			position -= horizontal_move; // �߂荞�񂾕���߂�
			velocity = XMVectorSetX(velocity, -XMVectorGetX(velocity)); // �ǂɂԂ������牡�����̑��x�𔽓]
			XMStoreFloat2(&e.position, position);
			e.isFacingRight = !e.isFacingRight; // �ǂɓ�������������𔽓]
		}

		//���E�V��Ƃ̓����蔻��
		XMVECTOR vertical_move = XMVectorSet(0.0f, XMVectorGetY(velocity) * (float)elapsed_time, 0.0f, 0.0f);
		position += vertical_move;
		XMStoreFloat2(&e.position, position);

		if (Map_hitJudgementBoxVSMap(Enemy_GetBoxCollision(i))) {
			position -= vertical_move; //�߂荞�񂾕���߂�
			velocity = XMVectorSetY(velocity, 0.0f); // �V�䂩�n�ʂɂԂ�������c�����̑��x��0��
			XMStoreFloat2(&e.position, position);
		}

		//�������̑��x��������𔻒f
		if (XMVectorGetX(velocity) > 0.0f) e.isFacingRight = true;
		else if (XMVectorGetX(velocity) < 0.0f) e.isFacingRight = false;

		XMStoreFloat2(&e.position, position);
		XMStoreFloat2(&e.velocity, velocity);


		//HP���炷����
		if (e.hp_red > e.hp) {

			//�������Ԃ�HP�o�[�����炷
			e.hp_red -= 4.0f * (float)elapsed_time;
			if (e.hp_red < e.hp) {
				e.hp_red = e.hp;
			}
		}
		else {
			e.hp_red = e.hp;
		}
	}
}

void Enemy_Draw() {
	int i = 0;
	XMFLOAT2 offset = Map_GetWorldOffset();

	for (Enemy& e : g_Enemys) {
		//�g���Ă��Ȃ�������ǂݔ�΂�
		if (!e.isEnable) continue;

		//���G���Ԃ�������_�ł�����
		if (e.InvincibleTime > 0.0f) {
			if ((int)(e.InvincibleTime * 10.0f) % 2 == 0) {
				continue;
			}
		}

		float screen_x = e.position.x - offset.x;;
		float screen_y = e.position.y - offset.y;;

		if (e.AnimPlayId == -1) {
			Sprite_Draw(g_EnemyTypes[e.typeId].TexId,
				screen_x, screen_y,
				ENEMY_WIDTH, ENEMY_HEIGHT, 0, 0, 512, 512, e.isFacingRight,
				e.isDamage ? XMFLOAT4{ 1.0f,0.0f,0.0f,1.0f } : XMFLOAT4{ 1.0f,1.0f,1.0f,1.0f });
		}
		else {
			SpriteAnim_Draw(e.AnimPlayId, screen_x, screen_y, ENEMY_WIDTH, ENEMY_HEIGHT, e.isFacingRight);
		}

		//HP�\��
		float damage_bar_ratio = e.hp_red / (float)g_EnemyTypes[e.typeId].hp;
		Sprite_Draw(g_HpRedTexId, screen_x, screen_y, 120.0f * damage_bar_ratio, 15.0f);
		float hp_ratio = e.hp / (float)g_EnemyTypes[e.typeId].hp;
		Sprite_Draw(g_HpBarTexId, screen_x, screen_y, 120.0f * hp_ratio, 15.0f);

		e.isDamage = false;

#if defined(DEBUG)||defined(_DEBUG)
		//Collision_DebugDraw(Enemy_GetBoxCollision(i));
		i++;
#endif
	}

}


void Enemy_Create(EnemyTypeID id, const DirectX::XMFLOAT2& position, bool GotoRight) {
	for (Enemy& e : g_Enemys) {
		if (e.isEnable) continue;

		//�󂫗̈攭������1�̕��쐬
		e.isEnable = true;
		e.typeId = id;
		e.offsetY = position.y;
		e.position = position;
		e.velocity = { -100.0f,0.0f };
		e.lifeTime = 0.0;
		e.TimetoFire = 0.0;
		e.InvincibleTime = 0.0f;
		e.hp = (float)g_EnemyTypes[e.typeId].hp;
		e.hp_red = e.hp;
		e.isDamage = false;
		e.isKnockback = false;
		if (g_EnemyTypes[id].AnimId == -1) {
			e.AnimPlayId = -1;
		}
		else {
			e.AnimPlayId = SpriteAnim_CreatePlayer(g_EnemyTypes[id].AnimId);
		}
		e.state = PATROL;
		e.isFacingRight = GotoRight;

		break;
	}
}

bool Enemy_IsEnable(int index){
	return g_Enemys[index].isEnable;
}

Circle Enemy_GetCollision(int index)
{
	int id = g_Enemys[index].typeId;

	float cx = g_Enemys[index].position.x + g_EnemyTypes[id].collision.center.x;
	float cy = g_Enemys[index].position.y + g_EnemyTypes[id].collision.center.y;

	return { {cx,cy},g_EnemyTypes[id].collision.radius };
}

Box Enemy_GetBoxCollision(int index)
{
	int id = g_Enemys[index].typeId;

	float cx = g_Enemys[index].position.x + g_EnemyTypes[id].collision.center.x;
	float cy = g_Enemys[index].position.y + g_EnemyTypes[id].collision.center.y;

	return { {cx,cy},ENEMY_WIDTH *0.4f,ENEMY_HEIGHT * 0.4f };
}

XMFLOAT2 Get_EnemyPosition(int index){
	return g_Enemys[index].position;
}

void Enemy_Destroy(int index) {
	XMFLOAT2 EnemyPos= Get_EnemyPosition(index);
	XMFLOAT2 effect_pos = EnemyPos;
	effect_pos.x -= ENEMY_WIDTH / 2.0f;
	effect_pos.y -= ENEMY_HEIGHT / 2.0f;
	Effect_Create(effect_pos);
	Particle_Create(ParticleType::ENEMY_DEATH, { effect_pos.x + ENEMY_WIDTH ,effect_pos.y + ENEMY_HEIGHT});
	Particle_Create(ParticleType::ENEMY_DEATH, { effect_pos.x + ENEMY_WIDTH ,effect_pos.y + ENEMY_HEIGHT});

	g_Enemys[index].isEnable = false;
}

//�_���[�W�����߂�������������Ƀ_���[�W������Ȃǂ���
void Enemy_Damage(int index) {
	if (Player_GetState() == STATE_STRONGATTACKEND) g_Enemys[index].hp -= 3; //hp��1���炷
	else g_Enemys[index].hp--;

	g_Enemys[index].isDamage = true;
	g_Enemys[index].isKnockback = true;
	g_Enemys[index].KnockbackTimer = 0.3f;

	XMFLOAT2 KnockBackPower;
	KnockBackPower.x = Player_GetPosition().x > g_Enemys[index].position.x ? -300.0f : 300.0f;
	KnockBackPower.y = -300.0f;

	g_Enemys[index].velocity = KnockBackPower;

	if (g_Enemys[index].KnockbackTimer <= 0.0f) {
		g_Enemys[index].isKnockback = false;
	}
}

void Set_EnemyInvincibleTime(int index, float time) {
	g_Enemys[index].InvincibleTime = time;
}

float Get_EnemyInvincibleTime(int index) {
	return g_Enemys[index].InvincibleTime;
}

float Get_EnemyHp(int index){
	return g_Enemys[index].hp;
}

XMFLOAT2 Get_NearestTargetPosision(const DirectX::XMFLOAT2& player_position) {
	float nearestDistSq = FLT_MAX;
	XMFLOAT2 nearestEnemy = { 0.0f, 0.0f };

	for (Enemy& e : g_Enemys) {
		if (!e.isEnable) continue;
		if (e.InvincibleTime > 0.0f) continue;

		float dx = e.position.x - player_position.x;
		float dy = e.position.y - player_position.y;
		float distSq = dx * dx + dy * dy;

		if (distSq < nearestDistSq) {
			nearestDistSq = distSq;
			nearestEnemy = e.position;
		}
	}

	//�{�X�킾������{�X�����G�Ώ�
	if (Boss_IsAlive()) {
		XMFLOAT2 boss_position = Boss_GetPosition();
		float dx = boss_position.x - player_position.x;
		float dy = boss_position.y - player_position.y;
		float distSq = dx * dx + dy * dy;
		if (distSq < nearestDistSq) {
			nearestDistSq = distSq;
			nearestEnemy = boss_position;
		}
	}

	if (nearestDistSq != FLT_MAX) return nearestEnemy;
	else {
		//�G�����Ȃ���������̋�����Ԃ�
		if (Player_IsFacingRight()) return{ player_position.x + 800.0f,player_position.y }; 
		else return{ player_position.x - 800.0f,player_position.y };
	}
}
