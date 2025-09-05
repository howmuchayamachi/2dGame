/*==============================================================================

  ��ʑJ�ڐ���[scene.h]
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

//���݂̃V�[����j�����āA�V�����V�[����
void Scene_Change(Scene scene);
//���݂̃V�[���ɐV�����V�[�����d�˂�
void Scene_Push(Scene scene);
//��ԏ�̃V�[������菜��
void Scene_Pop();

#endif //SCENE_H