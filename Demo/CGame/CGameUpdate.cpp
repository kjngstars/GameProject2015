#include "../CGame.h"

void CGame::Update(float dt, CDXInput* const inputDevice)
{
#pragma region luigi
	this->luigi.Update(dt, inputDevice, &this->_map);
	CLuigiBulletManager::Update(dt, this->_camera, &this->_map);
#pragma endregion

#pragma region enemy
	if (this->luigi.IsNormalState())
		CEnemiesManager::Update(dt, &this->_map,
			CEnemiesManager::GetListEnemy());
#pragma endregion

	//camera
	_camera->update(
		this->luigi.GetPosition().x, this->luigi.GetPosition().y);

	this->fps = 1000.0f / dt;
}