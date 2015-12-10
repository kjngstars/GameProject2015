#include "CEnemiesManager.h"
#include "CEnemy0.h"
#include "../CLuigi/CLuigiBullet.h"

std::map<int, CEnemy*> CEnemiesManager::listEnemyAlive;
std::map<int, CEnemy*> CEnemiesManager::listEnemyDied;

// khởi tạo texture cho các enemy
void CEnemiesManager::InitializeTexture(IDirect3DDevice9* pD3DDevice)
{
	CEnemy0::InitializeTexture(pD3DDevice);
}

void CEnemiesManager::Destroy()
{
	for (auto &enemy : CEnemiesManager::listEnemyAlive)
		delete enemy.second;

	CEnemiesManager::listEnemyAlive.clear();

	for (auto &enemy : CEnemiesManager::listEnemyDied)
		delete enemy.second;

	CEnemiesManager::listEnemyDied.clear();

	// hủy các texture enemy
	CEnemy0::ReleaseTexture();
}

void CEnemiesManager::AddEnemy(EnemyInfo info)
{
	switch (info._type)
	{
	case 113:
		CEnemiesManager::listEnemyAlive.insert(
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
			(info._id, CEnemiesManager::listEnemyAlive[info._id]));

	return result;
}

void CEnemiesManager::Update(float elapsedTime, CMap* const pMap,
	CCamera* const pCamera, std::map<int, CEnemy*> listEnemyAlive)
{
	std::list<int> remove;

	/*std::list<EnemyInfo> listInfo = pMap->getListEnemyInViewPort();

	for (auto& info : listInfo)
		CEnemiesManager::listEnemyAlive[info._id]->Update(elapsedTime, pMap, listEnemyAlive);*/
#pragma region enemies còn sống
	for (auto& enemy : CEnemiesManager::listEnemyAlive)
	{
		enemy.second->isDied > 0 ?
			remove.push_back(enemy.first) :
			enemy.second->Update(elapsedTime, pMap, pCamera, listEnemyAlive);
	}

	for (int i : remove)
	{
		CEnemiesManager::listEnemyDied.insert(
			std::pair<int, CEnemy*>(
				i, CEnemiesManager::listEnemyAlive[i]));

		CEnemiesManager::listEnemyAlive.erase(i);
	}
#pragma endregion

#pragma region enemies đã ngủm
	remove.clear();
	
	for (auto& enemy : CEnemiesManager::listEnemyDied)
	{
		enemy.second->isDied < 0 ?
			remove.push_back(enemy.first) :
			enemy.second->Update(elapsedTime, pMap, pCamera, listEnemyAlive);
	}

	for (int i : remove)
	{
		delete CEnemiesManager::listEnemyDied[i];
		CEnemiesManager::listEnemyDied.erase(i);
	}
#pragma endregion
}

void CEnemiesManager::Render(ID3DXSprite* pSprite, CCamera* const pCamera)
{
	for (auto& enemy : CEnemiesManager::listEnemyAlive)
		enemy.second->Render(pSprite, pCamera);

	for (auto& enemy : CEnemiesManager::listEnemyDied)
		enemy.second->Render(pSprite, pCamera);
}