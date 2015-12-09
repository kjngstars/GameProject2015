#include "CEnemiesManager.h"
#include "CEnemy0.h"
#include "../CLuigi/CLuigiBullet.h"

std::map<int, CEnemy*> CEnemiesManager::listEnemy;

void CEnemiesManager::InitializeTexture(IDirect3DDevice9* pD3DDevice)
{
	CEnemy0::InitializeTexture(pD3DDevice);
}

void CEnemiesManager::Destroy()
{
	for (auto &enemy : CEnemiesManager::listEnemy)
		delete enemy.second;

	CEnemiesManager::listEnemy.clear();

	CEnemy0::ReleaseTexture();
}

void CEnemiesManager::AddEnemy(EnemyInfo info)
{
	switch (info._type)
	{
	case 113:
		CEnemiesManager::listEnemy.insert(
			std::pair<int, CEnemy*>(
				info._id,
				new CEnemy0(info)));
		break;

	default:
		break;
	}

}

std::map<int, CEnemy*> CEnemiesManager::GetListEnemyinViewport(CMap* const pMap)
{
	std::map<int, CEnemy*> result;

	std::list<EnemyInfo> listInfo = pMap->getListEnemyInViewPort();
	for (auto& info : listInfo)
		result.insert(
			std::pair<int, CEnemy*>
			(info._id, CEnemiesManager::listEnemy[info._id]));

	return result;
}

void CEnemiesManager::Update(float elapsedTime, CMap* const pMap,
	std::map<int, CEnemy*> listEnemy)
{
	std::list<int> remove;

	/*std::list<EnemyInfo> listInfo = pMap->getListEnemyInViewPort();

	for (auto& info : listInfo)
		CEnemiesManager::listEnemy[info._id]->Update(elapsedTime, pMap, listEnemy);*/

	for (auto& enemy : CEnemiesManager::listEnemy)
	{
		if (enemy.second->isDied)
			remove.push_back(enemy.first);
		else
			enemy.second->Update(elapsedTime, pMap, listEnemy);
	}

	for (int i : remove)
		CEnemiesManager::listEnemy.erase(i);
}

void CEnemiesManager::Render(ID3DXSprite* pSprite, CCamera* const pCamera)
{
	for (auto& enemy : CEnemiesManager::listEnemy)
		enemy.second->Render(pSprite, pCamera);
}