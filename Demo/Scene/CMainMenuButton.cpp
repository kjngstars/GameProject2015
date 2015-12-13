#include "CMainMenuButton.h"

CMainMenuButton::~CMainMenuButton()
{
	SAFE_RELEASE(this->pTexture);
}

void CMainMenuButton::Initialize(IDirect3DDevice9* pD3DDevice,
	D3DXVECTOR2 position, std::string fileName)
{
	D3DXIMAGE_INFO info;
	HR(D3DXGetImageInfoFromFile(fileName.c_str(), &info));

	this->width = info.Width / 2.0f;
	this->height = info.Height;

	this->sourceRect = { 0,0,this->width,this->height };

	HR(D3DXCreateTextureFromFileEx(
		pD3DDevice, fileName.c_str(),
		info.Width, info.Height, D3DX_DEFAULT, 0,
		D3DFMT_UNKNOWN, D3DPOOL_MANAGED,
		D3DX_DEFAULT, D3DX_DEFAULT,
		0, &info, 0, &this->pTexture));

	this->position = position;
}

void CMainMenuButton::Selected(bool select)
{
	select ?
		this->sourceRect.left = this->width :
		this->sourceRect.left = 0;

	this->sourceRect.right = this->sourceRect.left + this->width;
}

void CMainMenuButton::Update(float dt)
{
	if (!this->dieFlag)
	{
		this->color.a += 0.04f;

		this->scale.x = this->scale.y = 1 / this->color.a;

		if (this->color.a >= 1.0f)
		{
			this->dieFlag = true;

			this->scale.x = this->scale.y = this->color.a = 1.0f;
		}
	}
}

void CMainMenuButton::Render(ID3DXSprite* pSprite)
{
	MATRIX matTransform;

	D3DXVECTOR2 anchorPoint = D3DXVECTOR2(this->width / 2.0f,
		this->height / 2.0f);

	D3DXMatrixTransformation2D(
		&matTransform,
		&anchorPoint, 0, &this->scale,
		&anchorPoint, 0,
		&D3DXVECTOR2(this->position.x - anchorPoint.x,
			this->position.y - anchorPoint.y));
	pSprite->SetTransform(&matTransform);

	HR(pSprite->Draw(
		this->pTexture, &this->sourceRect,
		0, 0, this->color));
}