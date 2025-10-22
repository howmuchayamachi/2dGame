/*====================================================================================

�I�[�f�B�I����[Audio.h]

																Author	: Harada Ren
																Date	: 2025/07/09
--------------------------------------------------------------------------------------

======================================================================================*/
#ifndef AUDIO_H
#define AUDIO_H


void InitAudio();
void UninitAudio();

int LoadAudio(const char* FileName);
void UnloadAudio(int Index);
void UnloadAllAudio();
void PlayAudio(int Index, bool Loop = false, float volume = 1.0f);
void StopAudio(int Index);
void StopAllAudio();

//�Đ�����BGM�̉��ʍX�V
void Audio_UpdateBGMVolume();

#endif //AUDIO_H
