/*==============================================================================

   敵の発生制御 [enemy_spawner.cpp]
														 Author : Harada Ren
														 Date   : 2025/07/02
--------------------------------------------------------------------------------

==============================================================================*/
#include "enemy_spawner.h"
#include "DirectXMath.h"
using namespace DirectX;


struct EnemySpawn {
	XMFLOAT2 position;
	EnemyTypeID id;
	int count; //発生最大数
	double rate; //発生頻度
	double time; //経過時間
	double spawnTime; //スポーンした時間
	int spawnCount; //実際にスポーンした数
	bool isEnded; //表示し終わったか判定
	bool GotoRight; //右向きか
};

static constexpr unsigned int ENEMY_SPAWNER_MAX = 100;
static EnemySpawn g_EnemySpawners[ENEMY_SPAWNER_MAX]{};
static int g_Spawner_Count = 0;
static double g_Time = 0.0;


void EnemySpawner_Initialize() {
	g_Spawner_Count = 0;
	g_Time = 0.0;
}

void EnemySpawner_Finalize() {
}

void EnemySpawner_Update(double elapsed_time) {
	g_Time += elapsed_time;

	for (int i = 0; i < g_Spawner_Count;i++) {
		if (g_EnemySpawners[i].isEnded) continue;
		if (g_EnemySpawners[i].time > g_Time) break;

		//1体目を出すための代入
		if (g_EnemySpawners[i].spawnCount == 0) {
			g_EnemySpawners[i].spawnTime = g_Time - g_EnemySpawners[i].rate - 0.00001;
		}

		//現在の時間から前回スポーンした時間を引いた値が、発生間隔を超えていたら発生させる
		if (g_Time - g_EnemySpawners[i].spawnTime >= g_EnemySpawners[i].rate) {
			Enemy_Create(g_EnemySpawners[i].id, g_EnemySpawners[i].position, g_EnemySpawners[i].GotoRight);
			g_EnemySpawners[i].spawnCount++;

			if (g_EnemySpawners[i].spawnCount >= g_EnemySpawners[i].count) {
				g_EnemySpawners[i].isEnded = true;
			}
			g_EnemySpawners[i].spawnTime = g_Time;
		}
	}
}

void EnemySpawner_Create(const XMFLOAT2& position, EnemyTypeID id, double spawn_time, double spawn_rate, int spawn_count, bool gotoright) {
	if (g_Spawner_Count >= ENEMY_SPAWNER_MAX) return;
	EnemySpawn* pEs = &g_EnemySpawners[g_Spawner_Count];
	pEs->position = position;
	pEs->id = id;
	pEs->time = spawn_time;
	pEs->rate = spawn_rate;
	pEs->count = spawn_count;
	pEs->isEnded = false;
	pEs->spawnCount = 0;
	pEs->spawnTime = 0.0;
	pEs->GotoRight = gotoright;

	g_Spawner_Count++;
}
