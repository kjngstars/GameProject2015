#include "../CGame.h"

void CGame::Render()
{
	CGraphics::GetInstancePtr()->getD3dx9Device()->Clear(0, NULL,
		D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 255), 1.0f, 0);
	BEGIN_SCENE();
	BEGIN_DRAW();

	//load map
	_map.drawBackground();
	_map.drawObject();

#pragma region luigi
	this->luigi.Render(
		CGraphics::GetInstancePtr()->GetSprite(), this->_camera);
	CLuigiBulletManager::Render(
		CGraphics::GetInstancePtr()->GetSprite(), this->_camera);
#pragma endregion

#pragma region enemies
	CEnemiesManager::Render(
		CGraphics::GetInstancePtr()->GetSprite(), this->_camera);
	CSEPointManager::Render(
		CGraphics::GetInstancePtr()->GetSprite(), this->_camera);
#pragma endregion enemies

#pragma region text
	MATRIX matTransform;
	D3DXMatrixIdentity(&matTransform);
	CGraphics::GetInstancePtr()->GetSprite()->SetTransform(&matTransform);

	RECT formatRect = { 0,0,800,600 };

	std::string str =
		"FPS: " + std::to_string(this->fps) + "\n" +
		"State: " + std::to_string(this->luigi.GetState()) + "\n" +
		"MoveType: " + std::to_string(this->luigi.GetMoveType()) + "\n" +
		"DelayTime: " + std::to_string(this->luigi.GetDelayTime()) + "\n" +
		"Direction: " + std::to_string(this->luigi.GetDirection()) + "\n" +
		"Velocity: " + std::to_string(this->luigi.GetVelocity().x) + " " +
		std::to_string(this->luigi.GetVelocity().y) + "\n" +
		"Position: " + std::to_string(this->luigi.GetPosition().x) + " " +
		std::to_string(this->luigi.GetPosition().y) + "\n" +
		"Camera: " + std::to_string(this->_camera->getPointTransform(0, 0).x) + " " +
		std::to_string(this->_camera->getPointTransform(0, 0).y) + "\n";

	this->pDefaultFont->DrawText(
		CGraphics::GetInstancePtr()->GetSprite(),
		str.c_str(), -1, &formatRect, DT_NOCLIP,
		D3DCOLOR_XRGB(0, 0, 0));
#pragma endregion

	END_DRAW();
	END_SCENE();
	CGraphics::GetInstancePtr()->getD3dx9Device()->Present(nullptr, nullptr, nullptr, nullptr);
}