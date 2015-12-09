#include "../CGame.h"

void CGame::Update(float dt, CDXInput* const inputDevice)
{
#pragma region luigi
	this->luigi.Update(dt, inputDevice, &this->_map);

#pragma endregion

	if (this->luigi.IsNormalState())
	{
		CLuigiBulletManager::Update(dt, this->_camera, &this->_map);
		CEnemiesManager::Update(dt, &this->_map,
			CEnemiesManager::GetListEnemyAlive());
		CSEPointManager::Update(dt);
	}

	//camera
	_camera->update(
		this->luigi.GetPosition().x, this->luigi.GetPosition().y);

	this->fps = 1000.0f / dt;
}