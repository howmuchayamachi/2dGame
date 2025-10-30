/*====================================================================================

オーディオ処理[Audio.cpp]

																Author	: Harada Ren
																Date	: 2025/07/09
--------------------------------------------------------------------------------------

======================================================================================*/
#include <xaudio2.h>
#include <assert.h>
#include "audio.h"
#include "audio_manager.h"

#pragma comment(lib,"winmm.lib")

static IXAudio2* g_Xaudio{};
static IXAudio2MasteringVoice* g_MasteringVoice{};

struct AUDIO {
	IXAudio2SourceVoice* SourceVoice{};
	BYTE* SoundData{};

	int						Length{};
	int						PlayLength{};
};

static constexpr int AUDIO_MAX = 100;
static AUDIO g_Audio[AUDIO_MAX]{};

static IXAudio2SourceVoice* g_BGMAudio = nullptr;


void InitAudio() {
	// XAudio生成
	XAudio2Create(&g_Xaudio, 0);

	// マスタリングボイス生成
	g_Xaudio->CreateMasteringVoice(&g_MasteringVoice);

	for (AUDIO& audio : g_Audio) {
		audio.SoundData = nullptr;
		audio.SourceVoice = nullptr;
	}
}


void UninitAudio() {
	for (int i = 0; i < AUDIO_MAX; i++) {
		if (g_Audio[i].SourceVoice) {
			UnloadAudio(i);
		}
	}

	g_MasteringVoice->DestroyVoice();
	g_Xaudio->Release();
}


int LoadAudio(const char* FileName) {
	int index = -1;

	for (int i = 0; i < AUDIO_MAX; i++) {
		if (g_Audio[i].SourceVoice == nullptr) {
			index = i;
			break;
		}
	}

	if (index == -1)
		return -1;


	// サウンドデータ読込
	WAVEFORMATEX wfx = { 0 };

	{
		HMMIO hmmio = NULL;
		MMIOINFO mmioinfo = { 0 };
		MMCKINFO riffchunkinfo = { 0 };
		MMCKINFO datachunkinfo = { 0 };
		MMCKINFO mmckinfo = { 0 };
		UINT32 buflen;
		LONG readlen;


		hmmio = mmioOpen((LPSTR)FileName, &mmioinfo, MMIO_READ);
		assert(hmmio);

		riffchunkinfo.fccType = mmioFOURCC('W', 'A', 'V', 'E');
		mmioDescend(hmmio, &riffchunkinfo, NULL, MMIO_FINDRIFF);

		mmckinfo.ckid = mmioFOURCC('f', 'm', 't', ' ');
		mmioDescend(hmmio, &mmckinfo, &riffchunkinfo, MMIO_FINDCHUNK);

		if (mmckinfo.cksize >= sizeof(WAVEFORMATEX)) {
			mmioRead(hmmio, (HPSTR)&wfx, sizeof(wfx));
		}
		else {
			PCMWAVEFORMAT pcmwf = { 0 };
			mmioRead(hmmio, (HPSTR)&pcmwf, sizeof(pcmwf));
			memset(&wfx, 0x00, sizeof(wfx));
			memcpy(&wfx, &pcmwf, sizeof(pcmwf));
			wfx.cbSize = 0;
		}
		mmioAscend(hmmio, &mmckinfo, 0);

		datachunkinfo.ckid = mmioFOURCC('d', 'a', 't', 'a');
		mmioDescend(hmmio, &datachunkinfo, &riffchunkinfo, MMIO_FINDCHUNK);



		buflen = datachunkinfo.cksize;
		g_Audio[index].SoundData = new unsigned char[buflen];
		readlen = mmioRead(hmmio, (HPSTR)g_Audio[index].SoundData, buflen);


		g_Audio[index].Length = readlen;
		g_Audio[index].PlayLength = readlen / wfx.nBlockAlign;


		mmioClose(hmmio, 0);
	}


	// サウンドソース生成
	g_Xaudio->CreateSourceVoice(&g_Audio[index].SourceVoice, &wfx);
	assert(g_Audio[index].SourceVoice);


	return index;
}




void UnloadAudio(int Index) {
	if (Index < 0 || Index >= AUDIO_MAX || g_Audio[Index].SourceVoice == nullptr) return;

	if (g_Audio[Index].SourceVoice) {
		g_Audio[Index].SourceVoice->Stop();
		g_Audio[Index].SourceVoice->FlushSourceBuffers();
		g_Audio[Index].SourceVoice->DestroyVoice();
		g_Audio[Index].SourceVoice = nullptr;
	}
	if (g_Audio[Index].SoundData) {
		delete[] g_Audio[Index].SoundData;
		g_Audio[Index].SoundData = nullptr;
	}

	g_Audio[Index].Length = 0;
	g_Audio[Index].PlayLength = 0;
}

void UnloadAllAudio() {
	for (int i = 0; i < AUDIO_MAX; i++) {
		if (g_Audio[i].SoundData) {
			UnloadAudio(i);
		}
	}
}



void PlayAudio(int Index, bool Loop, float volume) {
	if (Index < 0 || Index >= AUDIO_MAX) return;

	g_Audio[Index].SourceVoice->Stop();
	g_Audio[Index].SourceVoice->FlushSourceBuffers();


	// バッファ設定
	XAUDIO2_BUFFER bufinfo;

	memset(&bufinfo, 0x00, sizeof(bufinfo));
	bufinfo.AudioBytes = g_Audio[Index].Length;
	bufinfo.pAudioData = g_Audio[Index].SoundData;
	bufinfo.PlayBegin = 0;
	bufinfo.PlayLength = g_Audio[Index].PlayLength;

	volume *= AudioManager_GetVolume();
	g_Audio[Index].SourceVoice->SetVolume(volume);

	// ループ設定
	if (Loop) {
		bufinfo.LoopBegin = 0;
		bufinfo.LoopLength = g_Audio[Index].PlayLength;
		bufinfo.LoopCount = XAUDIO2_LOOP_INFINITE;

		//ループならBGMとして記憶
		g_BGMAudio = g_Audio[Index].SourceVoice;
	}

	g_Audio[Index].SourceVoice->SubmitSourceBuffer(&bufinfo, NULL);


	// 再生
	g_Audio[Index].SourceVoice->Start();

}

void StopAudio(int Index) {
	if (g_Audio[Index].SourceVoice != nullptr) {
		g_Audio[Index].SourceVoice->Stop(0);
		g_Audio[Index].SourceVoice->FlushSourceBuffers();
	}
}

void StopAllAudio() {
	for (int i = 0; i < AUDIO_MAX; i++) {
		StopAudio(i);
	}
}

void Audio_UpdateBGMVolume() {
	if (g_BGMAudio != nullptr) {
		g_BGMAudio->SetVolume(AudioManager_GetVolume());
	}
}




