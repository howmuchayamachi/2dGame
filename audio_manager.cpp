/*==============================================================================

   âπó ä«óù [audio_manager.cpp]
														 Author : Harada Ren
														 Date   : 2025/08/06
--------------------------------------------------------------------------------

==============================================================================*/
#include "audio_manager.h"

static float g_MasterVolume = 0.5f;

void AudioManager_Initialize(){
	g_MasterVolume = 0.5f;
}

void AudioManager_Finalize(){
}

void AudioManager_SetVolume(float volume){
	if (volume < 0.0f) g_MasterVolume = 0.0f;
	else if (volume > 1.0f) g_MasterVolume = 1.0f;
	else g_MasterVolume = volume;
}

float AudioManager_GetVolume(){
	return g_MasterVolume;
}
