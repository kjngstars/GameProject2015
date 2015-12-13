#pragma once

#include "CEnemy.h"
#include"CSEPoint.h"
#include "../DX/dxaudio.h"

class CEnemiesManager
{
private:
	static std::map<int, CEnemy*> listEnemy;

	static CSound* pEnemyCollisionSound;

	static bool CheckEnemyToRemove(CEnemy* pEnemy,
		CCamera* const pCamera, std::list<EnemyInfo> listInfo);

public:
	static void Initialize(IDirect3DDevice9* pD3DDevice);
	static void Destroy();

	static void AddEnemy(EnemyInfo info);

	static void Update(float elapsedTime, CMap* const pMap,
		CCamera* const pCamera, std::map<int, CEnemy*> listEnemyAlive);
	static void Render(ID3DXSprite* pSprite, CCamera* const pCamera);

	static std::map<int, CEnemy*> GetListEnemyAlive()
	{
		return CEnemiesManager::listEnemy;
	}

	static void Playsound();
};