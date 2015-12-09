#pragma once

#include "CEnemy.h"

class CEnemiesManager
{
private:
	static std::map<int, CEnemy*> listEnemy;

public:
	static void InitializeTexture(IDirect3DDevice9* pD3DDevice);
	static void Destroy();

	static void AddEnemy(EnemyInfo info);

	static void Update(float elapsedTime, CMap* const pMap,
		std::map<int, CEnemy*> listEnemy);
	static void Render(ID3DXSprite* pSprite, CCamera* const pCamera);

	static std::map<int, CEnemy*> GetListEnemy()
	{
		return CEnemiesManager::listEnemy;
	}

	static std::map<int, CEnemy*> GetListEnemyinViewport(CMap* const pMap);
};