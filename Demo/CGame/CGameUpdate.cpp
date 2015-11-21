#include "../CGame.h"

void CGame::Update(float dt, CDXInput* inputDevice)
{
#pragma region luigi
	this->luigi.Update(dt, inputDevice);
#pragma endregion

	//camera

	_camera->update(_camera->_x, _camera->_y);

	
	if (inputDevice->KeyDown(DIK_K))
	{
		_camera->moveLeft();
	}

	if (inputDevice->KeyDown(DIK_L))
	{
		_camera->moveRight();
	}

}