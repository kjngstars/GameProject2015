#include "CMainMenu.h"

void CMainMenu::Initialize(IDirect3DDevice9* pD3DDevice)
{
	D3DXIMAGE_INFO info;
	HR(D3DXGetImageInfoFromFile("./Content/Scene/MainMenu.png", &info));
	HR(D3DXCreateTextureFromFileEx(
		pD3DDevice, "./Content/Scene/MainMenu.png",
		info.Width, info.Height, D3DX_DEFAULT, 0,
		D3DFMT_UNKNOWN, D3DPOOL_MANAGED,
		D3DX_DEFAULT, D3DX_DEFAULT,
		0, &info, 0, &this->pBackground));

	this->playButton.Initialize(pD3DDevice,
		D3DXVECTOR2(300, 200),
		"./Content/Scene/PlayButton.png");

	this->exitButton.Initialize(pD3DDevice,
		D3DXVECTOR2(500, 400),
		"./Content/Scene/ExitButton.png");
}

SceneResult CMainMenu::Update(float elapsedTime, CDXInput* const inputDevice)
{
	this->playButton.Selected(false);
	this->exitButton.Selected(false);

	this->playButton.Update(elapsedTime);
	this->exitButton.Update(elapsedTime);

	if ((inputDevice->KeyPress(DIK_UP) ||
		inputDevice->KeyPress(DIK_LEFT)) &&
		this->currentButton > 0)
		this->currentButton--;

	if ((inputDevice->KeyPress(DIK_DOWN) ||
		inputDevice->KeyPress(DIK_RIGHT)) &&
		this->currentButton < 1)
		this->currentButton++;

	switch (this->currentButton)
	{
	case 0:
		this->playButton.Selected(true);

		if (inputDevice->KeyPress(DIK_RETURN))
			return SceneResult::Gameplay;
		break;

	case 1:
		this->exitButton.Selected(true);

		if (inputDevice->KeyPress(DIK_RETURN))
			return SceneResult::Exit;
		break;
	}

	return SceneResult::None;
}

void CMainMenu::Render(ID3DXSprite* pSprite)
{
	HR(pSprite->Draw(
		this->pBackground, 0,
		0, 0, D3DCOLOR_XRGB(255, 255, 255)));

	this->playButton.Render(pSprite);
	this->exitButton.Render(pSprite);
}