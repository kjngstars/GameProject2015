#include "CEnemiesManager.h"
#include "CEnemy0.h"
#include "CEnemy1.h"
#include "../CLuigi/CLuigiBullet.h"

std::map<int, CEnemy*> CEnemiesManager::listEnemy;
CSound* CEnemiesManager::pEnemyCollisionSound = 0;

void CEnemiesManager::Initialize(IDirect3DDevice9* pD3DDevice)
{
	// khởi tạo texture cho các enemy
	CEnemy0::InitializeTexture(pD3DDevice);
	CEnemy1::InitializeTexture(pD3DDevice);

	CEnemiesManager::pEnemyCollisionSound =
		LoadSound("./Content/Sound/EnemyCollision.wav");
}

void CEnemiesManager::Destroy()
{
	for (auto &enemy : CEnemiesManager::listEnemy)
		delete enemy.second;

	CEnemiesManager::listEnemy.clear();

	// hủy các texture enemy
	CEnemy0::ReleaseTexture();
	CEnemy1::ReleaseTexture();

	SAFE_DELETE(CEnemiesManager::pEnemyCollisionSound);
}

bool CEnemiesManager::CheckEnemyToRemove(CEnemy* pEnemy,
	CCamera* const pCamera, std::list<EnemyInfo> listInfo)
{
	bool checkNonExist = true;

	for (auto& info : listInfo)
		if (pEnemy->id == info._id)
		{
			checkNonExist = false;
			break;
		}

	BoudingBox viewport(
		pCamera->_position.x, pCamera->_position.y,
		SCREEN_WIDTH, SCREEN_HEIGHT);

	return !viewport.Intersect(pEnemy->GetBox()) &&
		checkNonExist;
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

	case 114:
		CEnemiesManager::listEnemy.insert(
			std::pair<int, CEnemy*>(
				info._id,
				new CEnemy1(info)));
		break;

	default:
		break;
	}

}

void CEnemiesManager::Update(float elapsedTime, CMap* const pMap,
	CCamera* const pCamera, std::map<int, CEnemy*> listEnemy)
{
	std::list<int> remove;

	std::list<EnemyInfo> listInfo = pMap->getListEnemyInViewPort();

	for (auto& info : listInfo)
		if (CEnemiesManager::listEnemy.find(info._id) ==
			CEnemiesManager::listEnemy.end())
			CEnemiesManager::AddEnemy(info);

	for (auto& enemy : CEnemiesManager::listEnemy)
	{
		enemy.second->Update(elapsedTime, pMap, pCamera, listEnemy);

		if (CEnemiesManager::CheckEnemyToRemove(enemy.second, pCamera, listInfo))
			remove.push_back(enemy.second->id);
	}

	for (auto& id : remove)
	{
		delete CEnemiesManager::listEnemy[id];
		CEnemiesManager::listEnemy.erase(id);
	}
}

void CEnemiesManager::Render(ID3DXSprite* pSprite, CCamera* const pCamera)
{
	for (auto& enemy : CEnemiesManager::listEnemy)
		enemy.second->Render(pSprite, pCamera);
}

void CEnemiesManager::Playsound()
{
	PlaySound(CEnemiesManager::pEnemyCollisionSound);
}