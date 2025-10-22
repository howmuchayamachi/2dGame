/*==============================================================================

  �t�F�[�h�C���E�A�E�g����[fade.h]
														 Author : Harada Ren
														 Date   : 2025/07/10
--------------------------------------------------------------------------------

==============================================================================*/
#ifndef FADE_H
#define FADE_H

#include<DirectXMath.h>

void Fade_Initialize();
void Fade_Finalize();
void Fade_Update(double elapsed_time);
void Fade_Draw();

void Fade_Start(double time, bool IsFadeOut, DirectX::XMFLOAT3 color = { 0.0f,0.0f,0.0f });

enum FadeState : int
{
	FADE_STATE_NONE, //�������Ă��Ȃ�
	FADE_STATE_FINISHED_IN, //�t�F�[�h�C���I�����
	FADE_STATE_FINISHED_OUT, //�t�F�[�h�A�E�g�I�����
	FADE_STATE_IN, //�t�F�[�h�C����
	FADE_STATE_OUT, //�t�F�[�h�A�E�g��
	FADE_STATE_MAX
};

FadeState Fade_GetState();

#endif //FADE_H
