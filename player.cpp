/*==============================================================================

   �v���C���[���� [player.cpp]
														 Author : Harada Ren
														 Date   : 2025/06/27
--------------------------------------------------------------------------------

==============================================================================*/
#include "player.h"
#include "DirectXMath.h"
using namespace DirectX;
#include "texture.h"
#include "sprite.h"
#include "key_logger.h"
#include "bullet.h"
#include "direct3d.h"
#include "collision.h"
#include "Audio.h"
#include "audio_manager.h"

static XMFLOAT2 g_PlayerPosition{}; //�ʒu
static XMFLOAT2 g_PlayerVelocity{}; //���x
static int g_PlayerTexId = -1;

static constexpr float PLAYER_WIDTH = 128.0f;
static constexpr float PLAYER_HEIGHT = 128.0f;


static Circle g_PlayerCollision{ {60.0f,60.0f},60.0f };
static bool g_PlayerEnable = true;

static int g_GunSoundId = -1;


void Player_Initialize(const XMFLOAT2& position){
	g_PlayerPosition = position;
	g_PlayerVelocity = { 0.0f,0.0f };
	g_PlayerEnable = true;
	g_PlayerTexId = Texture_Load(L"resource/texture/player/hal_chang01.png");
	g_GunSoundId= LoadAudio("resource/audio/gun01.wav");
}

void Player_Finalize(){
	//UnloadAudio(g_GunSoundId);
}

void Player_Update(double elapsed_time){
	if (!g_PlayerEnable) return;

	//���Z�ł���\���̂ɕϊ�(float2����xm�^)
	XMVECTOR position=XMLoadFloat2(&g_PlayerPosition);
	XMVECTOR velocity = XMLoadFloat2(&g_PlayerVelocity);


	XMVECTOR direction{};

	if (KeyLogger_IsPressed(KK_W)) {
		direction += {0.0f, -1.0f};
	}
	if (KeyLogger_IsPressed(KK_S)) {
		direction += {0.0f, 1.0f};
	}
	if (KeyLogger_IsPressed(KK_A)) {
		direction += {-1.0f, 0.0f};
	}
	if (KeyLogger_IsPressed(KK_D)) {
		direction += {1.0f, 0.0f};
	}

	//�P�ʃx�N�g����
	direction = XMVector2Normalize(direction);

	//�����Ȃ�(�s�^���Ǝ~�܂�)
	//position += direction * 10.0f;

	//���x����
	//�Œ�t���[��
	/*velocity += direction * 2.0f;
	position += velocity;
	velocity *= 0.9f;*/
	
	//�σt���[��
	//elapsed_time�Őϕ�
	velocity += direction * 10000000.0f //��(�j���[�g��)�ŋ@�̂𓮂����Ă���
		/ 2500.0f //�d��(2.5kg)
		* (float)elapsed_time;

	position += velocity * (float)elapsed_time;

	velocity += -velocity * 4.0f * (float)elapsed_time;

	//(xm����float2�^)
	XMStoreFloat2(&g_PlayerPosition, position);
	XMStoreFloat2(&g_PlayerVelocity, velocity);
	

	//��ʒ[����o�Ȃ��悤�ɂ���
	/*
	if (g_PlayerPosition.x < 0) {
		g_PlayerPosition.x = 0;
	}
	if (g_PlayerPosition.x > Direct3D_GetBackBufferWidth() - PLAYER_WIDTH) {
		g_PlayerPosition.x = (float)(Direct3D_GetBackBufferWidth() - PLAYER_WIDTH);
	}
	if (g_PlayerPosition.y < 0) {
		g_PlayerPosition.y = 0;
	}
	if (g_PlayerPosition.y > Direct3D_GetBackBufferHeight() - PLAYER_HEIGHT) {
		g_PlayerPosition.y = (float)(Direct3D_GetBackBufferHeight() - PLAYER_HEIGHT);
	}
	*/



	//�e����
	if (KeyLogger_IsTrigger(KK_SPACE)) {
		//Bullet_Create({g_PlayerPosition.x,g_PlayerPosition.y});
		PlayAudio(g_GunSoundId);
	}
}

void Player_Draw(){
	if (!g_PlayerEnable) return;

	Sprite_Draw(g_PlayerTexId, 500, 500,
		PLAYER_WIDTH, PLAYER_HEIGHT);

#if defined(DEBUG)||defined(_DEBUG)
	//Collision_DebugDraw(Player_GetCollision());
#endif

	/*Box box{{g_PlayerPosition.x,g_PlayerPosition.y},PLAYER_WIDTH,PLAYER_HEIGHT};
	Collision_DebugDraw(box);*/
}

bool Player_IsEnable(){
	return g_PlayerEnable;
}

Circle Player_GetCollision(){
	float cx = g_PlayerPosition.x + g_PlayerCollision.center.x;
	float cy = g_PlayerPosition.y + g_PlayerCollision.center.y;

	return { {cx,cy},g_PlayerCollision.radius };
}

void Player_Destroy(){
	g_PlayerEnable = false;
}
