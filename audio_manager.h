/*==============================================================================

   âπó ä«óù [audio_manager.h]
														 Author : Harada Ren
														 Date   : 2025/08/06
--------------------------------------------------------------------------------

==============================================================================*/
#ifndef AUDIO_MANAGER_H
#define AUDIO_MANAGER_H


void AudioManager_Initialize();
void AudioManager_Finalize();
void AudioManager_SetVolume(float volume);
float AudioManager_GetVolume();


#endif //AUDIO_MANAGER_H
