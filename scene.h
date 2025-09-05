/*==============================================================================

  画面遷移制御[scene.h]
														 Author : Harada Ren
														 Date   : 2025/07/10
--------------------------------------------------------------------------------

==============================================================================*/
#ifndef SCENE_H
#define SCENE_H

void Scene_Initialize();
void Scene_Finalize();
void Scene_Update(double elapsed_time);
void Scene_Draw();


enum Scene {
	SCENE_TITLE,
	SCENE_VOLUME,
	SCENE_GAME,
	SCENE_PAUSE,
	SCENE_RESULT,
	SCENE_CLEAR,
	SCENE_MAX
};

Scene GetCurrentScene();

//現在のシーンを破棄して、新しいシーンへ
void Scene_Change(Scene scene);
//現在のシーンに新しいシーンを重ねる
void Scene_Push(Scene scene);
//一番上のシーンを取り除く
void Scene_Pop();

#endif //SCENE_H