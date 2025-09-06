/*====================================================================================

���C������[main.cpp]

																Author	: Harada Ren
																Date	: 2025/06/06
--------------------------------------------------------------------------------------

======================================================================================*/
#include <SDKDDKVer.h>
#define WIN32_LEAN_AND_MEAN //�R���p�C�����Ԃ������Ȃ�(�Â��v���O�������g��Ȃ�)
#include <Windows.h>
#include "game_window.h"
#include "direct3d.h"
#include "shader.h"
#include "sprite.h"
#include "Texture.h"
#include "debug_ostream.h"
#include "sprite_anim.h"
#include "fade.h"
#include "collision.h"
#include "debug_text.h"
#include <sstream>
#include "system_timer.h"
#include "Key_logger.h"
#include "mouse.h"
#include <Xinput.h>
#pragma comment(lib,"xinput.lib")
#include "scene.h"
#include "Audio.h"
#include "game.h"

#include "polygon.h"



//���C��
int APIENTRY WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int nCmdShow) {

	//COM���C�u�����̏�����
	(void)CoInitializeEx(nullptr, COINIT_MULTITHREADED);

	////GPI�X�P�[�����O
	SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

	HWND hWnd = GameWindow_Create(hInstance);

	SystemTimer_Initialize();
	KeyLogger_Initialize();
	Mouse_Initialize(hWnd);
	InitAudio();

	Direct3D_Initialize(hWnd); //�_�C���N�g3D�̏�����
	Shader_Initialize(Direct3D_GetDevice(), Direct3D_GetContext()); //���_�C���N�g3D�̏������̌�ɏ�����
	Texture_Initialize(Direct3D_GetDevice(), Direct3D_GetContext());
	Sprite_Initialize(Direct3D_GetDevice(), Direct3D_GetContext());
	Fade_Initialize();
	Scene_Initialize();

	Polygon_Initialize(Direct3D_GetDevice(), Direct3D_GetContext());

#if defined(DEBUG)||defined(_DEBUG)
	hal::DebugText dt(Direct3D_GetDevice(), Direct3D_GetContext(),
		L"resource/texture/consolab_ascii_512.png",
		Direct3D_GetBackBufferWidth(), Direct3D_GetBackBufferHeight(),
		0.0f, 0.0f, //����
		0, 0,
		0.0f, 16.0f); //����

	Collision_DebugInitialize(Direct3D_GetDevice(), Direct3D_GetContext());
#endif

	ShowWindow(hWnd, nCmdShow); //�w��̃E�B���h�E����ʂɕ\������
	UpdateWindow(hWnd); //�E�B���h�E�̒����X�V����

	//fps�E���s�t���[�����x�v���p
	double exec_last_time = SystemTimer_GetTime();
	double fps_last_time = exec_last_time;
	double current_time = 0.0;
	ULONG frame_count = 0;
	double fps = 0.0;

	//�Q�[�����[�v&���b�Z�[�W���[�v
	MSG msg;

	//�E�B���h�E�v���O�����̒��S	
	do {
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) { //�E�B���h�E���b�Z�[�W�����Ă�����
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else { //�Q�[���̏���

			//fps�v��
			current_time = SystemTimer_GetTime(); //�V�X�e���������擾
			double elapsed_time = current_time - fps_last_time; //fps�v���p�̌o�ߎ��Ԃ��v�Z

			if (elapsed_time >= 1.0) { //1�b���ƂɌv��
				fps = frame_count / elapsed_time;
				fps_last_time = current_time; //FPS�𑪒肵��������ۑ�
				frame_count = 0; //�J�E���g���N���A
			}

			//1/60�b���ƂɎ��s
			elapsed_time = current_time - exec_last_time;
			//if(elapsed_time>=(1.0/60.0)){ //60fps�ɌŒ�
			if (true) { //�ǂ��fps�l�ł��A�j���[�V�����̓����͌Œ�
				exec_last_time = current_time; //��������������ۑ�

				//�Q�[���̍X�V
				KeyLogger_Update();

				Mouse_State ms{};
				Mouse_GetState(&ms); //�}�E�X�̏�Ԏ擾

				
				Scene_Update(elapsed_time);
				
				SpriteAnim_Update(elapsed_time);//�o�ߎ��ԂŃQ�[�������
				Fade_Update(elapsed_time);

				//�Q�[���̕`��
				//��ʂ̃N���A
				Direct3D_Clear();
				Sprite_Begin();				
				Scene_Draw();
				Fade_Draw();

#if defined(DEBUG)||defined(_DEBUG)//�f�o�b�O�̎������L��
				std::stringstream ss;
				//ss << "fps:" << fps << std::endl;

				dt.SetText(ss.str().c_str());
				//dt.SetText("debug!\nwatashidayo! \n nye!!", { 0.0f,0.0f,1.0f,1.0f });

				dt.Draw();
				dt.Clear();
#endif

				//��ʂ̃X���b�v
				Direct3D_Present();

				frame_count++;
			}
		}
	} while (msg.message != WM_QUIT);

	
#if defined(DEBUG)||defined(_DEBUG)
	Collision_DebugFinalize();
#endif

	//�������Ƌt���Ɍ�Еt��
	Scene_Finalize();
	Fade_Finalize();
	Polygon_Finalize();
	Shader_Finalize();
	Sprite_Finalize();
	Texture_Finalize();
	Direct3D_Finalize();
	UninitAudio();
	Mouse_Finalize();

	return (int)msg.wParam;
}

