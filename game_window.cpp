/*====================================================================================

ウィンドウプロシージャ[game_window.cpp]

																Author	: Harada Ren
																Date	: 2025/06/06
--------------------------------------------------------------------------------------

======================================================================================*/

#include "game_window.h"
#include <algorithm> //std::maxを使う
#include "keyboard.h"
#include "mouse.h"
#include "game.h"


//ウィンドウ情報
static constexpr char WINDOW_CLASS[] = "GameWindow"; //メインウィンドウクラス名
static constexpr char TITLE[] = "GameWindow"; //タイトルバーのテキスト

static HWND g_hWnd=NULL;

//ウィンドウプロシージャ プロトタイプ宣言
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);



HWND GameWindow_Create(HINSTANCE hInstance)
{
	//ウィンドウクラスの登録
	WNDCLASSEX wcex{};

	//構造体に設定値を入れる
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.lpfnWndProc = WndProc; //WndProc(関数名のみ)は関数のアドレス
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, IDI_APPLICATION);
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = nullptr; //メニューは作らない
	wcex.lpszClassName = WINDOW_CLASS;
	wcex.hIconSm = LoadIcon(wcex.hInstance, IDI_APPLICATION);

	//OSにwcexの内容のウィンドウを作ることを教える
	RegisterClassEx(&wcex);

	//メインウィンドウの作成
	//RECT window_rect{ 0,0,SCREEN_WIDTH,SCREEN_HEIGHT };//left,top,right,bottom (矩形)
	int SCREEN_WIDTH = GetSystemMetrics(SM_CXSCREEN);
	int SCREEN_HEIGHT = GetSystemMetrics(SM_CYSCREEN);
	RECT window_rect{ 0,0,SCREEN_WIDTH,SCREEN_HEIGHT};

	//|or  &and  ^xor  ~not
	//↓最大化とサイズ変更ができない
	DWORD style = WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME ^ WS_MAXIMIZEBOX;
	//WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME & ~WS_MAXIMIZEBOX ,
	AdjustWindowRect(&window_rect, style, FALSE); //最終的なウィンドウのサイズを出してくれる

	//デスクトップのサイズを取得
	//※プライマリモニターの画面解像度を取得
	//GetSystemMetrics…ハードウェアの情報が色々取れる
	int desktop_width = GetSystemMetrics(SM_CXSCREEN);
	int desktop_height = GetSystemMetrics(SM_CYSCREEN);

	g_hWnd = CreateWindowEx(
		0,
		wcex.lpszClassName,
		0,

		// ウィンドウスタイルを WS_POPUP に変更
		// タイトルバーや枠線のない、完全な描画領域になる
		WS_POPUP,

		//ウィンドウの位置とサイズも、画面にピッタリ合わせる
		0, 0, // 画面の左上(0,0)に配置
		desktop_width, // 画面全体の横幅を取得
		desktop_height, // 画面全体の高さを取得

		NULL,
		NULL,
		hInstance,
		NULL
	);

	return g_hWnd;
}

HWND GameWindow_GetHWND(){
	return g_hWnd;
}

//ウィンドウプロシージャ
//マウス移動・クリック・ウィンドウ操作などあらゆる操作
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

	case WM_CLOSE: //×ボタンもしくはウィンドウを閉じるときに警告を出すなど(そのあとに閉じるようにして2段階方式に)
		if (MessageBox(hWnd, "本当に終了してよろしいですか?", "確認", MB_OKCANCEL | MB_DEFBUTTON2) == IDOK) {
			DestroyWindow(hWnd);
		}
		break;

	case WM_DESTROY: //ウィンドウの破棄メッセージ(ウィンドウを閉じる)
		PostQuitMessage(0); //WM_QUITメッセージの送信(GetMessageに0が入って終了する)
		break;
	default:
		//通常のメッセージ処理はこの関数に任せる
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}