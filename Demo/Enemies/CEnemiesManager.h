#pragma once

#include "CEnemy.h"
#include"CSEPoint.h"

class CEnemiesManager
{
private:
	static std::map<int, CEnemy*> listEnemyAlive;
	static std::map<int, CEnemy*> listEnemyDied;

public:
	static void InitializeTexture(IDirect3DDevice9* pD3DDevice);
	static void Destroy();

	static void AddEnemy(EnemyInfo info);

	static void Update(float elapsedTime, CMap* const pMap,
		std::map<int, CEnemy*> listEnemyAlive);
	static void Render(ID3DXSprite* pSprite, CCamera* const pCamera);

	static std::map<int, CEnemy*> GetListEnemyAlive()
	{
		return CEnemiesManager::listEnemyAlive;
	}

	static std::map<int, CEnemy*> GetListEnemyinViewport(CMap* const pMap);
};