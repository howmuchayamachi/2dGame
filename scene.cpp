/*==============================================================================

  画面遷移制御[scene.cpp]
														 Author : Harada Ren
														 Date   : 2025/07/11
--------------------------------------------------------------------------------

==============================================================================*/
#include "scene.h"
#include "title.h"
#include "volume.h"
#include "game.h"
#include "pause.h"
#include "result.h"
#include "clear.h"
#include <vector>
#include "map.h"
#include "texture.h"

static std::vector<Scene> g_SceneStack;

void Scene_Initialize(){
	g_SceneStack.clear();
	g_SceneStack.push_back(SCENE_TITLE);
	Title_Initialize();
}

void Scene_Finalize(){
	switch (GetCurrentScene()) {
	case SCENE_TITLE:
		Title_Finalize();
		break;

	case SCENE_VOLUME:
		Volume_Finalize();
		break;

	case SCENE_GAME:
		Game_Finalize();
		break;

	case SCENE_PAUSE:
		Pause_Finalize();
		break;

	case SCENE_RESULT:
		Result_Finalize();
		break;

	case SCENE_CLEAR:
		Clear_Finalize();
		break;

	default:
		break;
	}
}

void Scene_Update(double elapsed_time){
	switch (GetCurrentScene()) {
	case SCENE_TITLE:
		Title_Update();
		break;

	case SCENE_VOLUME:
		Volume_Update(elapsed_time);
		break;

	case SCENE_GAME:
		Game_Update(elapsed_time);
		break;

	case SCENE_PAUSE:
		Pause_Update();
		break;

	case SCENE_RESULT:
		Result_Update();
		break;

	case SCENE_CLEAR:
		Clear_Update();
		break;

	default:
		break;
	}
}

void Scene_Draw(){
	for (Scene scene : g_SceneStack) {
		switch (scene) {
		case SCENE_TITLE:
			Title_Draw();
			break;

		case SCENE_VOLUME:
			Volume_Draw();
			break;

		case SCENE_GAME:
			Game_Draw();
			break;

		case SCENE_PAUSE:
			Pause_Draw();
			break;

		case SCENE_RESULT:
			Result_Draw();
			break;

		case SCENE_CLEAR:
			Clear_Draw();
			break;

		default:
			break;
		}
	}
}

//現在のシーンを返す
Scene GetCurrentScene() {
	return g_SceneStack.back(); //vectorの末尾
}

void Scene_Change(Scene scene){
	//スタックをすべてクリアして新しいシーンを追加
	while (!g_SceneStack.empty()) {
		Scene_Pop();
	}
	Scene_Push(scene);
}

void Scene_Push(Scene scene){
	g_SceneStack.push_back(scene);

	switch (GetCurrentScene()){
	case SCENE_TITLE:
		Title_Initialize();
		Game_SetCheckPoint(false);
		Game_SetPlayerPosition({ 10.0f * MAPCHIP_WIDTH,15.0f * MAPCHIP_HEIGHT });
		break;

	case SCENE_VOLUME:
		Volume_Initialize();
		break;

	case SCENE_GAME:
		Game_Initialize();
		break;

	case SCENE_PAUSE:
		Pause_Initialize();
		break;

	case SCENE_RESULT:
		Result_Initialize();
		break;

	case SCENE_CLEAR:
		Clear_Initialize();
		break;

	default:
		break;
	}
}

void Scene_Pop(){
	if (g_SceneStack.empty()) return;

	Scene_Finalize();

	g_SceneStack.pop_back();
}
