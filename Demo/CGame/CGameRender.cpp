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


	this->luigi.Render(CGraphics::GetInstancePtr()->GetSprite());

#pragma region text
	MATRIX matTransform;
	D3DXMatrixIdentity(&matTransform);
	CGraphics::GetInstancePtr()->GetSprite()->SetTransform(&matTransform);

	RECT formatRect = { 0,0,800,600 };

	std::string str =
		"State: " + std::to_string(this->luigi.GetState()) + "\n" +
		"MoveType: " + std::to_string(this->luigi.GetMoveType()) + "\n" +
		"DelayTime: " + std::to_string(this->luigi.GetDelayTime()) + "\n" +
		"Direction: " + std::to_string(this->luigi.GetDirection()) + "\n" +
		"Velocity: " + std::to_string(this->luigi.GetVelocity().x) + " " +
		std::to_string(this->luigi.GetVelocity().y) + "\n";

	this->pDefaultFont->DrawText(
		CGraphics::GetInstancePtr()->GetSprite(),
		str.c_str(), -1, &formatRect, DT_NOCLIP,
		D3DCOLOR_XRGB(0, 0, 0));
#pragma endregion

	END_DRAW();
	END_SCENE();
	CGraphics::GetInstancePtr()->getD3dx9Device()->Present(nullptr, nullptr, nullptr, nullptr);
}