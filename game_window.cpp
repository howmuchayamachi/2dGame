/*====================================================================================

�E�B���h�E�v���V�[�W��[game_window.cpp]

																Author	: Harada Ren
																Date	: 2025/06/06
--------------------------------------------------------------------------------------

======================================================================================*/

#include "game_window.h"
#include <algorithm> 
#include "keyboard.h"
#include "mouse.h"
#include "game.h"


//�E�B���h�E���
static constexpr char WINDOW_CLASS[] = "GameWindow"; //���C���E�B���h�E�N���X��
static constexpr char TITLE[] = "GameWindow"; //�^�C�g���o�[�̃e�L�X�g

static HWND g_hWnd = NULL;

//�E�B���h�E�v���V�[�W��
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);



HWND GameWindow_Create(HINSTANCE hInstance){
	//�E�B���h�E�N���X�̓o�^
	WNDCLASSEX wcex{};

	//�\���̂ɐݒ�l������
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.lpfnWndProc = WndProc; //WndProc(�֐����̂�)�͊֐��̃A�h���X
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, IDI_APPLICATION);
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = nullptr; //���j���[�͍��Ȃ�
	wcex.lpszClassName = WINDOW_CLASS;
	wcex.hIconSm = LoadIcon(wcex.hInstance, IDI_APPLICATION);

	//OS��wcex�̓��e�̃E�B���h�E����邱�Ƃ�������
	RegisterClassEx(&wcex);

	//���C���E�B���h�E�̍쐬
	int SCREEN_WIDTH = GetSystemMetrics(SM_CXSCREEN);
	int SCREEN_HEIGHT = GetSystemMetrics(SM_CYSCREEN);
	RECT window_rect{ 0,0,SCREEN_WIDTH,SCREEN_HEIGHT };

	DWORD style = WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME ^ WS_MAXIMIZEBOX;
	AdjustWindowRect(&window_rect, style, FALSE);

	//�f�X�N�g�b�v�̃T�C�Y���擾
	int desktop_width = GetSystemMetrics(SM_CXSCREEN);
	int desktop_height = GetSystemMetrics(SM_CYSCREEN);

	g_hWnd = CreateWindowEx(
		0,
		wcex.lpszClassName,
		0,
		WS_POPUP,
		0, 0, // ��ʂ̍���(0,0)�ɔz�u
		desktop_width, // ��ʑS�̂̉������擾
		desktop_height, // ��ʑS�̂̍������擾
		NULL,
		NULL,
		hInstance,
		NULL
	);

	return g_hWnd;
}

HWND GameWindow_GetHWND() {
	return g_hWnd;
}

//�E�B���h�E�v���V�[�W��
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
	case WM_ACTIVATEAPP:
		Keyboard_ProcessMessage(message, wParam, lParam);
		Mouse_ProcessMessage(message, wParam, lParam);
		break;
	case WM_INPUT:
	case WM_MOUSEMOVE:
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_MBUTTONDOWN:
	case WM_MBUTTONUP:
	case WM_MOUSEWHEEL:
	case WM_XBUTTONDOWN:
	case WM_XBUTTONUP:
	case WM_MOUSEHOVER:
		Mouse_ProcessMessage(message, wParam, lParam);
		break;
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
	case WM_KEYUP:
	case WM_SYSKEYUP:
		Keyboard_ProcessMessage(message, wParam, lParam);
		break;

	case WM_CLOSE:
		if (MessageBox(hWnd, "�{���ɏI�����Ă�낵���ł���?", "�m�F", MB_OKCANCEL | MB_DEFBUTTON2) == IDOK) {
			DestroyWindow(hWnd);
		}
		break;

	case WM_DESTROY: //�E�B���h�E�̔j�����b�Z�[�W(�E�B���h�E�����)
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}