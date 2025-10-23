/*==============================================================================

   弾制御 [bullet.cpp]
														 Author : Harada Ren
														 Date   : 2025/07/01
--------------------------------------------------------------------------------

==============================================================================*/
#include "bullet.h"
#include "DirectXMath.h"
using namespace DirectX;
#include "direct3d.h"
#include "texture.h"
#include "sprite.h"
#include "collision.h"
#include "map.h"
#include "Player.h"
#include "particle.h"
#include "boss.h"

struct Bullet {
	XMFLOAT2 position;
	double LifeTime; //存在時間
	bool isEnable; //使っているかどうか
	bool isGoingRight; //右向きか
	Circle collision;
	float angle;

	//ベジェ曲線用変数
	double duration; //着弾までの時間
	XMFLOAT2 P0; //スタート地点
	XMFLOAT2 P1; //中間点
	XMFLOAT2 P2; //ゴール地点
};

struct EnemyBullet {
	XMFLOAT2 position;
	double LifeTime; //存在時間
	bool isEnable; //使っているかどうか
	bool isGoingRight; //右向きか
	Circle collision;
	float angle;
	bool willExplosion; //ボスの必殺技か

	//ベジェ曲線用変数
	double duration; //着弾までの時間
	XMFLOAT2 P0; //スタート地点
	XMFLOAT2 P1; //中間点
	XMFLOAT2 P2; //ゴール地点
};

static Bullet g_Bullets[BULLET_MAX]{};
static EnemyBullet g_EnemyBullets[BULLET_MAX]{};

static int g_BulletTexId1 = -1;
static int g_EnemyBulletTexId1 = -1;
static int g_ParticleBulletTexId = -1;

static constexpr float BULLET_SPEED = 2000.0f;
static constexpr float ENEMY_BULLET_SPEED = 1000.0f;

void Bullet_Initialize() {
	for (Bullet& b : g_Bullets) {
		b.isEnable = false;
	}
	for (EnemyBullet& eb : g_EnemyBullets) {
		eb.isEnable = false;
	}
	g_BulletTexId1 = Texture_Load(L"resource/texture/bullet1.png");
	g_EnemyBulletTexId1 = Texture_Load(L"resource/texture/enemy_bullet1.png");
	g_ParticleBulletTexId = Texture_Load(L"resource/texture/effect000.jpg");
}

void Bullet_Finalize() {
}

void Bullet_Update(double elapsed_time) {
	for (Bullet& b : g_Bullets) {
		//使っていなかったら読み飛ばす
		if (!b.isEnable) continue;

		b.LifeTime += elapsed_time;
		//着弾したら消す
		if (b.LifeTime >= b.duration) {
			b.isEnable = false;
		}

		XMFLOAT2 prev_position = b.position;

		// 時間の進行度 t を計算 (0.0 ~ 1.0)
		float t = (float)(b.LifeTime / b.duration);

		// 2次ベジェ曲線の公式
		// P(t) = (1-t)^2 * P0 + 2(1-t)t * P1 + t^2 * P2
		XMVECTOR p0 = XMLoadFloat2(&b.P0);
		XMVECTOR p1 = XMLoadFloat2(&b.P1);
		XMVECTOR p2 = XMLoadFloat2(&b.P2);

		XMVECTOR term1 = (1 - t) * (1 - t) * p0;
		XMVECTOR term2 = 2 * (1 - t) * t * p1;
		XMVECTOR term3 = t * t * p2;

		XMVECTOR current_pos = term1 + term2 + term3;
		XMStoreFloat2(&b.position, current_pos);

		Particle_Create(ParticleType::PLAYER_BULLET, b.position);

		//前の位置と今の位置へのベクトルを計算して、角度を変更
		XMVECTOR prev_pos_vec = XMLoadFloat2(&prev_position);
		XMVECTOR current_pos_vec = XMLoadFloat2(&b.position);
		XMVECTOR velocity_vec = current_pos_vec - prev_pos_vec;

		//    ベクトルの長さが0だと計算できないので、少しでも動いていたら計算する
		if (XMVectorGetX(XMVector2LengthSq(velocity_vec)) > 0.001f) {
			b.angle = atan2f(XMVectorGetY(velocity_vec), XMVectorGetX(velocity_vec));
		}
	}

	for (EnemyBullet& eb : g_EnemyBullets) {
		//使っていなかったら読み飛ばす
		if (!eb.isEnable) continue;

		eb.LifeTime += elapsed_time;
		//着弾したら消す
		if (eb.LifeTime >= eb.duration) {
			eb.isEnable = false;
		}

		XMFLOAT2 prev_position = eb.position;

		// 時間の進行度 t を計算 (0.0 ~ 1.0)
		float t = (float)(eb.LifeTime / eb.duration);

		// 2次ベジェ曲線の公式
		// P(t) = (1-t)^2 * P0 + 2(1-t)t * P1 + t^2 * P2
		XMVECTOR p0 = XMLoadFloat2(&eb.P0);
		XMVECTOR p1 = XMLoadFloat2(&eb.P1);
		XMVECTOR p2 = XMLoadFloat2(&eb.P2);

		XMVECTOR term1 = (1 - t) * (1 - t) * p0;
		XMVECTOR term2 = 2 * (1 - t) * t * p1;
		XMVECTOR term3 = t * t * p2;

		XMVECTOR current_pos = term1 + term2 + term3;
		XMStoreFloat2(&eb.position, current_pos);

		Particle_Create(ParticleType::ENEMY_BULLET, { eb.position.x + 32.0f, eb.position.y + 32.0f });

		//前の位置と今の位置へのベクトルを計算して、角度を変更
		XMVECTOR prev_pos_vec = XMLoadFloat2(&prev_position);
		XMVECTOR current_pos_vec = XMLoadFloat2(&eb.position);
		XMVECTOR velocity_vec = current_pos_vec - prev_pos_vec;

		//  ベクトルの長さが0だと計算できないので、少しでも動いていたら計算する
		if (XMVectorGetX(XMVector2LengthSq(velocity_vec)) > 0.001f) {
			eb.angle = atan2f(XMVectorGetY(velocity_vec), XMVectorGetX(velocity_vec));
		}
	}
}

void Bullet_Draw() {
	Direct3D_SetAlphaBlendAdd();

	XMFLOAT2 offset = Map_GetWorldOffset();

	for (Bullet& b : g_Bullets) {
		//使っていなかったら読み飛ばす
		if (!b.isEnable) continue;

		float screen_x = b.position.x - offset.x;
		float screen_y = b.position.y - offset.y;

		Sprite_Draw(g_BulletTexId1, screen_x, screen_y, 128.0f, 128.0f, 0, 0, 2048, 2048, b.angle);
	}

	Direct3D_SetAlphaBlendTransparent();
}

void EnemyBullet_Draw() {
	Direct3D_SetAlphaBlendAdd();

	XMFLOAT2 offset = Map_GetWorldOffset();

	for (EnemyBullet& eb : g_EnemyBullets) {
		//使っていなかったら読み飛ばす
		if (!eb.isEnable) continue;

		float screen_x = eb.position.x - offset.x;
		float screen_y = eb.position.y - offset.y;

		if (Get_BossState() == BOSS_KINGS_DROP || Get_BossState() == BOSS_KINGS_EXPLOSION) {
			Sprite_Draw(g_ParticleBulletTexId, screen_x + 32.0f, screen_y + 32.0f, 64.0f, 64.0f, { 0.5f,0.5f,1.0f,1.0f });
		}
		else {
			Sprite_Draw(g_EnemyBulletTexId1, screen_x, screen_y,
				128.0f, 128.0f, 0, 0, 1024, 1024, eb.angle, { 0.5f,0.1f,1.0f,1.0f });
		}
	}

	Direct3D_SetAlphaBlendTransparent();
}

void Bullet_Create(const XMFLOAT2& position, const XMFLOAT2 target_position) {
	for (Bullet& b : g_Bullets) {
		if (b.isEnable) continue;

		//空き領域発見して1発分作成
		b.isEnable = true;
		b.LifeTime = 0.0;
		b.position = position;
		b.collision = { {64.0f,64.0f},32.0f };

		b.P0 = b.position;
		b.P2 = target_position;

		XMVECTOR p0_vec = XMLoadFloat2(&b.P0);
		XMVECTOR p2_vec = XMLoadFloat2(&b.P2);

		float distance = XMVectorGetX(XMVector2Length(p2_vec - p0_vec));

		b.duration = (float)(distance / BULLET_SPEED);

		// 軌道の基準となる点を計算するための変数
		float t = 1.0f / 4.0f;
		XMVECTOR base_point = p0_vec * (1.0f - t) + p2_vec * t;
		float curve_height = 150.0f; // カーブの膨らみ具合（上に膨らむ）

		// スタートからゴールへのベクトル
		XMVECTOR vec_p0_to_p2 = p2_vec - p0_vec;

		XMVECTOR up_direction = XMVectorSet(0.0f, -1.0f, 0.0f, 0.0f);

		// 垂直方向にずらしてP1を決定
		XMVECTOR p1_vec = base_point + up_direction * curve_height;

		XMStoreFloat2(&b.P1, p1_vec);

		break;
	}
}

void EnemyBullet_Create(const DirectX::XMFLOAT2& position, const XMFLOAT2 target_position, bool isBossReinforced, bool isKingsDrop) {
	for (EnemyBullet& eb : g_EnemyBullets) {
		if (eb.isEnable) continue;

		static int bullet_num = 1;

		//空き領域発見して1発分作成
		eb.isEnable = true;
		eb.LifeTime = 0.0;
		eb.position = position;
		eb.collision = { {64.0f,64.0f},32.0f };

		if(isKingsDrop) eb.willExplosion = true;
		else eb.willExplosion = false;

		eb.P0 = eb.position;
		eb.P2 = target_position;

		XMVECTOR p0_vec = XMLoadFloat2(&eb.P0);
		XMVECTOR p2_vec = XMLoadFloat2(&eb.P2);

		float distance = XMVectorGetX(XMVector2Length(p2_vec - p0_vec));

		eb.duration = (float)(distance / ENEMY_BULLET_SPEED);

		float t = 1.0f / 4.0f;
		XMVECTOR base_point = p0_vec * (1.0f - t) + p2_vec * t;

		float curve_height = 200.0f;
		if (isBossReinforced) {
			if (isKingsDrop) {
				curve_height = 100.0f;
				eb.duration = 3.0;
			}
			else {
				switch (bullet_num) {
				case 1:
					curve_height = 400.0f;
					break;

				case 2:
					curve_height = 200.0f;
					break;

				case 3:
					curve_height = -200.0f;
					break;

				case 4:
					curve_height = -400.0f;
					break;

				default:
					break;
				}
			}
		}

		XMVECTOR vec_p0_to_2 = p2_vec - p0_vec;

		XMVECTOR up_direction = XMVectorSet(0.0f, -1.0f, 0.0f, 0.0f);

		XMVECTOR p1_vec = base_point + up_direction * curve_height;
		XMStoreFloat2(&eb.P1, p1_vec);

		if (!isBossReinforced) break;
		else {
			if (bullet_num >= 4) {
				bullet_num = 1;
				break;
			}
			bullet_num++;
		}
	}
}

bool Bullet_IsEnable(int index) {
	return g_Bullets[index].isEnable;
}

bool EnemyBullet_IsEnable(int index) {
	return g_EnemyBullets[index].isEnable;
}

Circle Bullet_GetCollision(int index) {
	float cx = g_Bullets[index].collision.center.x + g_Bullets[index].position.x;
	float cy = g_Bullets[index].collision.center.y + g_Bullets[index].position.y;

	return { {cx,cy},g_Bullets[index].collision.radius };
}

Circle EnemyBullet_GetCollision(int index) {
	float cx = g_EnemyBullets[index].collision.center.x + g_EnemyBullets[index].position.x;
	float cy = g_EnemyBullets[index].collision.center.y + g_EnemyBullets[index].position.y;

	return { {cx,cy},g_EnemyBullets[index].collision.radius };
}



void Bullet_Destroy(int index) {
	g_Bullets[index].isEnable = false;

}

void EnemyBullet_Destroy(int index) {
	g_EnemyBullets[index].isEnable = false;
}

bool Bullet_WillExplosion(int index){
	return g_EnemyBullets[index].willExplosion;
}
