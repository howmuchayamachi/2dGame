/*==============================================================================

   �}�b�v�ړ��L�����N�^�[���� [runner.cpp]
														 Author : Harada Ren
														 Date   : 2025/07/17
--------------------------------------------------------------------------------

==============================================================================*/
#include "runner.h"
#include "DirectXMath.h"
using namespace DirectX;
#include "texture.h"
#include "sprite.h"
#include "sprite_anim.h"
#include "key_logger.h"
#include "bullet.h"
#include "collision.h"
#include "direct3d.h"
#include "map.h"
#include "Audio.h"
#include "audio_manager.h"
#include "enemy.h"
#include "mouse.h"
#include "player_ui.h"
#include "particle.h"

static int g_RunnerTexId = -1;
static int AnimId_Player = -1;
static int PlayId_Player = -1;

static int g_AttackTexId = -1;
static int AnimId_Attack = -1;
static int PlayId_Attack = -1;

static int g_AttackEndTexId = -1;
static int AnimId_AttackEnd = -1;
static int PlayId_AttackEnd = -1;

static int g_StrongAttackEndTexId = -1;
static int AnimId_StrongAttackEnd = -1;
static int PlayId_StrongAttackEnd = -1;


static XMFLOAT2 g_RunnerPosition{}; //world���W
static XMFLOAT2 g_RunnerVelocity{}; //���x

static XMFLOAT2 g_PrevRunnerPosition{}; //�O�̃t���[���̈ʒu���L������

static Circle g_RunnerCollision{ {60.0f,60.0f},40.0f };
static bool g_RunnerEnable = true;

//�E�������Ă�����true ����������false
static bool g_IsRunnerFacingRight = true;

static int g_GunSoundId = -1;

static bool g_IsJump = false;

static PLAYER_STATE g_PlayerState = STATE_STOP;

static double g_OnewayIgnoreTimer = 0.0;

static double g_coyoteTimer = 0.0;

static double g_AttackHoldTime = 0.0;

static int g_BladeAudioId = -1;
static int g_ChargeAudioId = -1;
static int g_DamageSEId = -1;

static float g_RunnerHp = RUNNER_MAXHP;
static float g_RunnerInvincibleTime = 0.0f;

static float g_RunnerMp = RUNNER_MAXMP;

static double g_StrongAttackTimer = 0.0;

static int g_StrongAttackShotCount = 0;
static double g_StrongAttackShotIntervalTimer = 0.0;

static constexpr double SHOT_INTERVAL = 0.15;

static OBB g_AttackCollisions[6];

void Runner_Initialize(const XMFLOAT2& position) {
	g_RunnerTexId = Texture_Load(L"resource/texture/player/hal_chang_anim.png");
	g_AttackTexId = Texture_Load(L"resource/texture/player/hal_chang_attack_anim.png");
	g_AttackEndTexId = Texture_Load(L"resource/texture/player/hal_chang_attack_end_anim.png");
	g_StrongAttackEndTexId = Texture_Load(L"resource/texture/player/hal_chang_strongattack_end_anim.png");

	AnimId_Player = SpriteAnim_RegisterPattern(g_RunnerTexId, 16, 8, 0.07, { 1024,1024 }, { 0,0 });
	PlayId_Player = SpriteAnim_CreatePlayer(AnimId_Player);

	AnimId_Attack = SpriteAnim_RegisterPattern(g_AttackTexId, 4, 4, 0.03, { 2048,2048 }, { 0,0 }, false);
	PlayId_Attack = SpriteAnim_CreatePlayer(AnimId_Attack);

	AnimId_AttackEnd = SpriteAnim_RegisterPattern(g_AttackEndTexId, 7, 7, 0.03, { 2048,2048 }, { 0,0 }, false);
	PlayId_AttackEnd = SpriteAnim_CreatePlayer(AnimId_AttackEnd);

	AnimId_StrongAttackEnd = SpriteAnim_RegisterPattern(g_StrongAttackEndTexId, 9, 5, 0.05, { 2048,2048 }, { 0,0 }, false);
	PlayId_StrongAttackEnd = SpriteAnim_CreatePlayer(AnimId_StrongAttackEnd);

	g_RunnerPosition = position;
	g_RunnerVelocity = { 0.0f,0.0f };
	g_RunnerEnable = true;

	g_PlayerState = STATE_STOP;

	g_IsRunnerFacingRight = true;

	g_BladeAudioId = LoadAudio("resource/audio/blade1.wav");
	g_ChargeAudioId = LoadAudio("resource/audio/charge.wav");
	g_DamageSEId = LoadAudio("resource/audio/damage.wav");

	g_RunnerHp = RUNNER_MAXHP;
	g_RunnerInvincibleTime = 0.0f;

	g_RunnerMp = RUNNER_MAXMP;

	g_OnewayIgnoreTimer = 0.0;
	g_coyoteTimer = 0.0;

	g_StrongAttackTimer = 0.0;

	g_StrongAttackShotCount = 0;
	g_StrongAttackShotIntervalTimer = 0.0;

	g_AttackCollisions[0] = { {0.0f, 0.0f}, {0.0f, 0.0f}, {{1,0}, {0,1}} };
	g_AttackCollisions[1] = { {0.0f, -90.0f}, {70.0f, 30.0f}, {{1,0}, {0,1}} };
	g_AttackCollisions[2] = { {30.0f, -70.0f}, {70.0f, 30.0f}, {{1,0}, {0,1}} };
	g_AttackCollisions[3] = { {60.0f, -50.0f}, {70.0f, 30.0f}, {{1,0}, {0,1}} };
	g_AttackCollisions[4] = { {80.0f, -10.0f}, {70.0f, 30.0f}, {{1,0}, {0,1}} };
	g_AttackCollisions[5] = { {100.0f, 40.0f}, {70.0f, 30.0f}, {{1,0}, {0,1}} };
}

void Runnner_Finalize() {
}

void Runner_Update(double elapsed_time) {
	if (!g_RunnerEnable) return;

	//���Ԍo�ߏ���
	if (g_RunnerInvincibleTime > 0.0f) {
		g_RunnerInvincibleTime -= (float)elapsed_time;
	}

	if (g_coyoteTimer > 0.0) {
		g_coyoteTimer -= elapsed_time;
	}

	//�ԃQ�[�W���Ȃ����HP,MP����
	if (g_RunnerHp<RUNNER_MAXHP && !isHpDecrease()) {
		g_RunnerHp += 0.5f * (float)elapsed_time;
		if (g_RunnerHp >= RUNNER_MAXHP) {
			g_RunnerHp = RUNNER_MAXHP;
		}
	}
	if (g_RunnerMp<RUNNER_MAXMP && !isMpDecrease()) {
		g_RunnerMp += 1.0f * (float)elapsed_time;
		if (g_RunnerMp >= RUNNER_MAXMP) {
			g_RunnerMp = RUNNER_MAXMP;
		}
	}



	//�O�̈ʒu��ۑ�
	g_PrevRunnerPosition = g_RunnerPosition;

	// ���Z�ł���\���̂ɕϊ�
	XMVECTOR position = XMLoadFloat2(&g_RunnerPosition);
	XMVECTOR velocity = XMLoadFloat2(&g_RunnerVelocity);

	// �n��ł̍��E�ړ��𑬓x�Ő���
	if (!g_IsJump) {
		float move_speed = 500.0f; // �n��ł̈ړ��X�s�[�h
		float dash_speed = 800.0f; //�_�b�V�����̈ړ��X�s�[�h
		if (g_PlayerState == STATE_RUN) {
			move_speed = dash_speed;
		}
		if (KeyLogger_IsPressed(KK_D)) {
			velocity = XMVectorSetX(velocity, move_speed);
			if (g_PlayerState != STATE_ATTACKEND && g_PlayerState != STATE_STRONGATTACKEND) {
				g_IsRunnerFacingRight = true;
			}
		}
		else if (KeyLogger_IsPressed(KK_A)) {
			velocity = XMVectorSetX(velocity, -move_speed);
			if (g_PlayerState != STATE_ATTACKEND && g_PlayerState != STATE_STRONGATTACKEND) {
				g_IsRunnerFacingRight = false;
			}
		}
		else {
			// �L�[��������ĂȂ���Ή������̑��x��0�ɂ���
			velocity = XMVectorSetX(velocity, 0.0f);
		}
	}
	else{
		// �󒆂ł̈ړ�����
		float aerial_control_force = 1500.0f; // �󒆂ō��E�ɉ�����͂̑傫��
		float max_aerial_speed = 500.0f;      // �󒆂ł̉������̍ō����x
		if (g_PlayerState == STATE_RUN) {
			max_aerial_speed = 800.0f;
		}
		// �L�[���͂ɉ����āA���E�ɗ͂�������i�����x�j
		if (KeyLogger_IsPressed(KK_D)) {
			velocity += XMVectorSet(aerial_control_force * (float)elapsed_time, 0.0f, 0.0f, 0.0f);
			if (g_PlayerState != STATE_ATTACKEND && g_PlayerState != STATE_STRONGATTACKEND) {
				g_IsRunnerFacingRight = true;
			}
		}
		if (KeyLogger_IsPressed(KK_A)) {
			velocity -= XMVectorSet(aerial_control_force * (float)elapsed_time, 0.0f, 0.0f, 0.0f);
			if (g_PlayerState != STATE_ATTACKEND && g_PlayerState != STATE_STRONGATTACKEND) {
				g_IsRunnerFacingRight = false;
			}
		}

		// �󒆂ł̉����x���ō����x�𒴂��Ȃ��悤�ɒ�������
		float vx = XMVectorGetX(velocity);
		if (vx > max_aerial_speed) {
			velocity = XMVectorSetX(velocity, max_aerial_speed);
		}
		if (vx < -max_aerial_speed) {
			velocity = XMVectorSetX(velocity, -max_aerial_speed);
		}
	}

	// �����E�F�C������~��鏈��
	if (g_OnewayIgnoreTimer > 0.0) {
		g_OnewayIgnoreTimer -= elapsed_time;
	}
	if (g_coyoteTimer > 0.0 && KeyLogger_IsPressed(KK_S) && KeyLogger_IsTrigger(KK_SPACE)) {
		g_IsJump = true;
		//�������^�C�}�[���Z�b�g
		g_OnewayIgnoreTimer = 0.2;
		g_coyoteTimer = 0.0;
	}



	// �W�����v
	// �R���[�e�^�C���ȓ���������W�����v�\
	if (KeyLogger_IsTrigger(KK_SPACE) && g_coyoteTimer>0.0) {
		float jump_power = -1000.0f; // Y�����̃W�����v��
		// Y�����̑��x���W�����v�͂ŏ㏑��
		velocity = XMVectorSetY(velocity, jump_power);
		g_IsJump = true;
		g_coyoteTimer = 0.0;
	}

	// �d��
	XMFLOAT2 gdir{ 0.0f, 1.0f };
	float gravity = 9.8f * 200.0f * (float)elapsed_time;
	velocity += XMLoadFloat2(&gdir) * gravity;



	// �����蔻��
	// �������Ɉړ����ĕǂƂ̓����蔻��
	XMVECTOR horizontal_move = XMVectorSet(XMVectorGetX(velocity) * (float)elapsed_time, 0.0f, 0.0f, 0.0f);
	position += horizontal_move;
	XMStoreFloat2(&g_RunnerPosition, position);

	if (Map_hitJudgementBoxVSMap(Runner_GetBoxCollision())) {
		position -= horizontal_move; // �߂荞�񂾕���߂�
		velocity = XMVectorSetX(velocity, 0.0f); // �������̑��x��0��
		XMStoreFloat2(&g_RunnerPosition, position);
	}
	else {
		if (XMVectorGetY(velocity) > 0.0f) {
			g_IsJump = true;
		}
	}

	// �c�����Ɉړ����Ēn��/�V��Ƃ̓����蔻��
	XMVECTOR vertical_move = XMVectorSet(0.0f, XMVectorGetY(velocity) * (float)elapsed_time, 0.0f, 0.0f);
	position += vertical_move;
	XMStoreFloat2(&g_RunnerPosition, position);

	Box player_box = Runner_GetBoxCollision();

	if (Map_hitJudgementBoxVSMap(player_box)) {
		position -= vertical_move; // �߂荞�񂾕���߂�

		// �������ɒn�ʂɂԂ������璅�n�Ƃ݂Ȃ�
		if (XMVectorGetY(velocity) > 0) {
			g_IsJump = false;
			g_coyoteTimer = 0.1;
		}

		velocity = XMVectorSetY(velocity, 0.0f); // �V�䂩�n�ʂɂԂ�������c�����̑��x��0��
		XMStoreFloat2(&g_RunnerPosition, position);
	}
	else if (g_OnewayIgnoreTimer <= 0.0) {
		// ����p�̈ꎞ�ϐ�
		int player_map_y = Map_GetWorldToMapY(player_box.center.y + player_box.half_height);
		float platform_top_y = (float)player_map_y * MAPCHIP_HEIGHT;
		float player_bottom_y = player_box.center.y + player_box.half_height;

		// �v���C���[�̑����Ƀ����E�F�C������pBox���쐬
		Box foot_box = player_box;
		foot_box.center.y = player_bottom_y;
		foot_box.half_height = 5.0f;
		foot_box.half_width = 10.0f;


		if (Map_hitJudgementBoxVSOneway(foot_box)) {
			// ���������A�O�t���[���ő���������ɂ������璅�n
			if (XMVectorGetY(velocity) >= 0 && (g_PrevRunnerPosition.y + player_box.half_height) <= platform_top_y) {
				position -= vertical_move; // �߂荞�񂾕���߂�
				g_IsJump = false;
				velocity = XMVectorSetY(velocity, 0.0f);
				g_coyoteTimer = 0.1;
			}
		}
	}

	// �ŏI�I�ȑ��x�ƈʒu��ۑ�
	XMStoreFloat2(&g_RunnerVelocity, velocity);
	XMStoreFloat2(&g_RunnerPosition, position);




	//�v���C���[�̃X�e�[�g�Ǘ�

	// �U�����łȂ���΍U�����J�n�ł���
	if (KeyLogger_IsTrigger(KK_J)) {
		if (g_PlayerState < STATE_ATTACK) {
			g_PlayerState = STATE_ATTACK;
			SpriteAnim_DestroyPlayer(PlayId_Attack);
			PlayId_Attack = SpriteAnim_CreatePlayer(AnimId_Attack);
		}
	}

	//�U���{�^���������Ă���ԃ^�C�}�[��i�߂�
	if (g_PlayerState == STATE_ATTACK) {
		if (KeyLogger_IsPressed(KK_J)) {
			g_PlayerState = STATE_ATTACK;
			g_AttackHoldTime += elapsed_time;
			
			//0.9�b�z�[���h�����狭�U���Ɉڍs
			if (g_AttackHoldTime > 0.9) {
				g_PlayerState = STATE_STRONGATTACK;
				PlayAudio(g_ChargeAudioId);
			}
		}
	}

	//�`���[�W���p�[�e�B�N������
	if (g_PlayerState == STATE_STRONGATTACK) {
		Particle_Create(ParticleType::CHARGE, { g_RunnerPosition.x + RUNNER_WIDTH / 2.0f, g_RunnerPosition.y + RUNNER_HEIGHT / 2.0f });
	}

	//�U���{�^�����b���ꂽ�u�Ԃɂǂ̍U�����o��������
	if (KeyLogger_IsReleased(KK_J)) {
		//0.9�b������Ă����狭�U��
		if (g_PlayerState == STATE_STRONGATTACK) {
			g_PlayerState = STATE_STRONGATTACKEND;

			g_StrongAttackShotCount = 0;
			g_StrongAttackShotIntervalTimer = 0.0;

			//�J������h�炷
			Map_StartShake(10.0f, 0.5);

			// �U���A�j�����Đ����钼�O�Ƀ��Z�b�g
			SpriteAnim_DestroyPlayer(PlayId_StrongAttackEnd);
			PlayId_StrongAttackEnd = SpriteAnim_CreatePlayer(AnimId_StrongAttackEnd);
			PlayAudio(g_BladeAudioId);
		}
		//�P����(0.9�b�ȓ�)�ł���Βʏ�U��
		else if (g_PlayerState == STATE_ATTACK) {
			g_PlayerState = STATE_ATTACKEND;
			SpriteAnim_DestroyPlayer(PlayId_AttackEnd);
			PlayId_AttackEnd = SpriteAnim_CreatePlayer(AnimId_AttackEnd);
			PlayAudio(g_BladeAudioId);
		}
		g_AttackHoldTime = 0.0;
	}


	//�U�����E�U���I����
	if (g_PlayerState == STATE_ATTACKEND) {
		if (SpriteAnim_IsStopped(PlayId_AttackEnd)) {
			g_PlayerState = STATE_STOP;

			if (g_RunnerMp >= 1.0f) {
				//�e��1������
				XMFLOAT2 bullet_pos;
				if (g_IsRunnerFacingRight) {
					// �E�����̎��̓v���C���[�̉E���ɒe���o��
					bullet_pos = { g_RunnerPosition.x + 80.0f, g_RunnerPosition.y - 50.0f };
				}
				else {
					// �������̎��̓v���C���[�̍����ɒe���o��
					bullet_pos = { g_RunnerPosition.x - 30.0f, g_RunnerPosition.y - 50.0f };
				}

				Bullet_Create({ bullet_pos.x,bullet_pos.y }, Get_NearestTargetPosision(g_RunnerPosition));
				
				g_RunnerMp -= 1.0f;
			}

		}
	}
	else if (g_PlayerState == STATE_STRONGATTACKEND) {
		//���U���̌o�ߎ���
		g_StrongAttackTimer += elapsed_time;
		//�A�˗p�^�C�}�[
		g_StrongAttackShotIntervalTimer += elapsed_time;

		//�e��3������
		if (g_RunnerMp >= 0.5f) {
			if (g_StrongAttackShotCount < 3 && g_StrongAttackShotIntervalTimer >= SHOT_INTERVAL) {
				//�e����
				XMFLOAT2 bullet_pos;
				if (g_IsRunnerFacingRight) {
					bullet_pos = { g_RunnerPosition.x + 60.0f + 30.0f * (float)g_StrongAttackShotCount,
						g_RunnerPosition.y - 70.0f + 30.0f * (float)g_StrongAttackShotCount };
				}
				else {
					bullet_pos = { g_RunnerPosition.x - 10.0f - 30.0f * (float)g_StrongAttackShotCount,
						g_RunnerPosition.y - 70.0f + 30.0f * (float)g_StrongAttackShotCount };
				}
				Bullet_Create(bullet_pos, Get_NearestTargetPosision(g_RunnerPosition));
				g_RunnerMp -= 0.5f;

				g_StrongAttackShotCount++;
				g_StrongAttackShotIntervalTimer = 0.0;
			}
		}

		if (SpriteAnim_IsStopped(PlayId_StrongAttackEnd)) {
			g_PlayerState = STATE_STOP;
		}
	}
	else {
		//��Ԃ��ς�����烊�Z�b�g
		g_StrongAttackTimer = 0.0;
	}

	// �U�����łȂ���΁A�ړ���_�b�V���̏�Ԃ��`�F�b�N	
	if (!g_IsJump && g_PlayerState < STATE_ATTACK) {
		if (KeyLogger_IsPressed(KK_LEFTSHIFT)) {
			g_PlayerState = STATE_RUN;
		}
		else if (KeyLogger_IsPressed(KK_D) || KeyLogger_IsPressed(KK_A)) {
			g_PlayerState = STATE_WALK;
		}
		else {
			g_PlayerState = STATE_STOP;
		}
	}
}


void Runner_Draw() {
	if (!g_RunnerEnable) return;

	// �J�����̃I�t�Z�b�g���擾
	XMFLOAT2 offset = Map_GetWorldOffset();

	// �v���C���[�̃��[���h���W����X�N���[�����W���v�Z
	float screen_x = g_RunnerPosition.x - offset.x;
	float screen_y = g_RunnerPosition.y - offset.y;

	//���G���Ԃ�������_�ł�����
	if (g_RunnerInvincibleTime > 0.0f) {
		if ((int)(g_RunnerInvincibleTime * 10.0f) % 2 == 0) {
			return;
		}
	}

	switch (g_PlayerState) {
	case STATE_STOP:
		Sprite_Draw(g_RunnerTexId, screen_x, screen_y, 128.0f, 128.0f, 0, 0, 1024, 1024, !g_IsRunnerFacingRight);
		break;

	case STATE_WALK:
		SpriteAnim_Draw(PlayId_Player, screen_x, screen_y, RUNNER_WIDTH, RUNNER_HEIGHT, !g_IsRunnerFacingRight);
		break;

	case STATE_RUN:
		SpriteAnim_Draw(PlayId_Player, screen_x, screen_y, RUNNER_WIDTH, RUNNER_HEIGHT, !g_IsRunnerFacingRight);
		break;

	case STATE_ATTACK:
		if (g_IsRunnerFacingRight) {
			SpriteAnim_Draw(PlayId_Attack, screen_x, screen_y - 128.0f, 256.0f, 256.0f);
		}
		else {
			SpriteAnim_Draw(PlayId_Attack, screen_x - 128.0f, screen_y - 128.0f, 256.0f, 256.0f, true);
		}
		break;

	case STATE_ATTACKEND:
		if (g_IsRunnerFacingRight) {
			SpriteAnim_Draw(PlayId_AttackEnd, screen_x, screen_y - 128.0f, 256.0f, 256.0f);
		}
		else {
			SpriteAnim_Draw(PlayId_AttackEnd, screen_x - 128.0f, screen_y - 128.0f, 256.0f, 256.0f, true);
		}
		break;

	case STATE_STRONGATTACK:
		if (g_IsRunnerFacingRight) {
			Sprite_Draw(g_StrongAttackEndTexId, screen_x, screen_y - 384.0f, 512.0f, 512.0f, 0, 0, 2048, 2048);
		}
		else {
			Sprite_Draw(g_StrongAttackEndTexId, screen_x - 384.0f, screen_y - 384.0f, 512.0f, 512.0f, 0, 0, 2048, 2048, true);
		}

		break;

	case STATE_STRONGATTACKEND:
		if (g_IsRunnerFacingRight) {
			SpriteAnim_Draw(PlayId_StrongAttackEnd, screen_x, screen_y - 384.0f, 512.0f, 512.0f);
		}
		else {
			SpriteAnim_Draw(PlayId_StrongAttackEnd, screen_x - 384.0f, screen_y - 384.0f, 512.0f, 512.0f, true);
		}
		break;

	default:
		break;
	}
}

bool Runner_IsEnable(){
	return g_RunnerEnable;
}

Circle Runner_GetCollision(){
	float cx = g_RunnerPosition.x + (RUNNER_WIDTH * 0.5f);
	float cy = g_RunnerPosition.y + (RUNNER_HEIGHT * 0.5f);
	return { {cx,cy},g_RunnerCollision.radius };
}

Box Runner_GetBoxCollision(){
	//���S���W
	float hw = RUNNER_WIDTH * 0.5f;
	float hh = RUNNER_HEIGHT * 0.5f;
	return { {g_RunnerPosition.x + hw,g_RunnerPosition.y + hh + 10},hw - 30,hh - 10 };
}

OBB Runner_GetAttackCollision(){	
	// �U�����ȊO�͓����蔻��Ȃ�
	if (g_PlayerState != STATE_ATTACKEND && g_PlayerState != STATE_STRONGATTACKEND) {
		return { {0,0}, {0,0}, {{1,0},{0,1}} };
	}

	int current_frame = -1;
	if (g_PlayerState == STATE_ATTACKEND) {
		current_frame = SpriteAnim_GetPatternNum(PlayId_AttackEnd);
	}
	else {
		// ���U���p�̓����蔻��ƃA�j���[�V����ID���g��
		current_frame = SpriteAnim_GetPatternNum(PlayId_StrongAttackEnd);
	}
	
	OBB local_obb = g_AttackCollisions[current_frame];
	if (local_obb.half_extent.x == 0) return local_obb; // �����蔻��Ȃ��t���[��

	//���U����������g�傷��
	if (g_PlayerState == STATE_STRONGATTACKEND) {
		OBB strong_local_obb = local_obb;
		strong_local_obb.half_extent.x *= 3.0f;
		strong_local_obb.half_extent.y *= 1.5f;

		//�g��ł��ꂽ�������炷�@�@
		strong_local_obb.center.x += (strong_local_obb.half_extent.x - local_obb.half_extent.x) / 2.0f;
		strong_local_obb.center.y -= (strong_local_obb.half_extent.y - local_obb.half_extent.y) / 2.0f;

		local_obb = strong_local_obb;
	}

	// ���̊p�x���t���[���ɍ��킹�ĕς���
	float angle = XM_PI;
	switch (current_frame) {
	case 1:
		angle /= -2.0f; // ��90�x
		break;

	case 2:
		angle /= -3.0f; //��60�x
		break;

	case 3:
		angle /= -4.0f; //��45�x
		break;

	case 4:
		angle /= -6.0f; //��30�x
		break;

	case 5:
		angle = 0.0f; //0�x
		break;

	default:
		break;
	}

	// �v���C���[���������Ȃ�A���[�J�����W�Ɗp�x�𔽓]
	if (!g_IsRunnerFacingRight) {
		local_obb.center.x *= -1.0f;
		angle *= -1.0f;
	}

	// �p�x����OBB�̎��x�N�g�����v�Z
	float c = cosf(angle);
	float s = sinf(angle);
	local_obb.axis[0] = { c, s };
	local_obb.axis[1] = { -s, c };

	// ���[���h���W�ɕϊ�
	local_obb.center.x += g_RunnerPosition.x + (RUNNER_WIDTH * 0.5f);
	local_obb.center.y += g_RunnerPosition.y + (RUNNER_HEIGHT * 0.5f);

	return local_obb;
}

void Runner_Destroy() {
	g_RunnerEnable = false;
}

DirectX::XMFLOAT2 Runner_GetPosition() {
	return g_RunnerPosition;
}


void Runner_Damage(float damage) {
	if (g_RunnerInvincibleTime > 0.0f) {
		return;
	}

	g_RunnerHp -= damage;
	if (g_RunnerHp < 0) {
		g_RunnerHp = 0;
	}

	PlayAudio(g_DamageSEId);
	g_RunnerInvincibleTime = 1.0f;
}

float Runner_GetHp() {
	return g_RunnerHp;
}

float Runner_GetMp(){
	return g_RunnerMp;
}

bool Runner_IsDead() {
	return g_RunnerHp <= 0.0f;
}

PLAYER_STATE Runner_GetState(){
	return g_PlayerState;
}

bool Runner_IsFacingRight(){
	return g_IsRunnerFacingRight;
}
