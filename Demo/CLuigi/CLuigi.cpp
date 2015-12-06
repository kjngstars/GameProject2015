#include "CLuigi.h"
#include "LuigiConstant.h"
#include "CSPoint.h"

CLuigi::~CLuigi()
{
	SAFE_RELEASE(this->pEffect);
	SAFE_RELEASE(this->luigiCloth);
	CSPointManager::Destroy();

	if (this->pJumpSound) delete this->pJumpSound;
	if (this->pGrowUpToBigSound) delete this->pGrowUpToBigSound;
	if (this->pGrowUpToFireSound) delete this->pGrowUpToFireSound;
	if (this->pFireSound) delete this->pFireSound;
}

void CLuigi::Initialize(IDirect3DDevice9* pD3DDevice)
{
	D3DXIMAGE_INFO info;
	HR(D3DXGetImageInfoFromFile("./Content/Luigi/Luigi.png", &info));
	HR(D3DXCreateTextureFromFileEx(
		pD3DDevice, "./Content/Luigi/Luigi.png",
		info.Width, info.Height, D3DX_DEFAULT, 0,
		D3DFMT_UNKNOWN, D3DPOOL_MANAGED,
		D3DX_DEFAULT, D3DX_DEFAULT,
		0, &info, 0, &this->_texture));

	HR(D3DXGetImageInfoFromFile("./Content/Luigi/LuigiCloth.png", &info));
	HR(D3DXCreateTextureFromFileEx(
		pD3DDevice, "./Content/Luigi/LuigiCloth.png",
		info.Width, info.Height, D3DX_DEFAULT, 0,
		D3DFMT_UNKNOWN, D3DPOOL_MANAGED,
		D3DX_DEFAULT, D3DX_DEFAULT,
		0, &info, 0, &this->luigiCloth));

	ID3DXBuffer* errors = 0;
	D3DXCreateEffectFromFile(
		pD3DDevice, "./Content/Luigi/E0.fx",
		0, 0, D3DXSHADER_DEBUG, 0, &this->pEffect, &errors);
	if (errors)
		MessageBox(0, (char*)errors->GetBufferPointer(), 0, 0);

	HR(this->pEffect->SetTexture("_texture", this->_texture));
	HR(this->pEffect->SetTexture("luigiCloth", this->luigiCloth));
	HR(this->pEffect->SetTechnique("DefaultTechnique"));

#pragma region Sound
	this->pJumpSound = LoadSound("./Content/Sound/Jump.wav");
	this->pGrowUpToBigSound = LoadSound("./Content/Sound/GrowUpToBig.wav");
	this->pGrowUpToFireSound = LoadSound("./Content/Sound/GrowUpToFire.wav");
	this->pFireSound = LoadSound("./Content/Sound/Fire.wav");
	this->pDieSound = LoadSound("./Content/Sound/Die.wav");
#pragma endregion

	this->_rotate = 0.0f;
	this->_scale = { 1.0f, 1.0f };
	this->_position = { 0.0f, 600.0f };

	this->type = LuigiType::Small;
	this->state = LuigiState::Normal;
	this->moveType = LuigiMoveType::Jumping;

	this->delayTime = LUIGI_DELAYTIME1;
	this->elapsedTime = 0.0f;
	this->ghostTime = 0.0f;
	this->invincibleTime = 0.0f;
	this->fDelayTime = 0.0f;

	this->index = 0;
	this->frame = 0;

	this->sourceRect = { 0,0,LUIGI_WSRCRECT_SMALL,LUIGI_HSRCRECT_SMALL };
	this->color = { 255,255,255,255 };
	this->hue = 0.0f;

	this->direction = LUIGI_DIRECTION_RIGHT;
	this->velocity = { 0.0f,0.0f };

	CSPointManager::Initialize(pD3DDevice);
}

void CLuigi::OnLostDevice()
{
	this->pEffect->OnLostDevice();
}

void CLuigi::OnResetDevice()
{
	this->pEffect->OnResetDevice();
}

void CLuigi::GrowUpToBig()
{
	if (this->type != LuigiType::Big &&
		this->type != LuigiType::Fire)
	{
		this->state = LuigiState::GrowToBig;
		this->type = LuigiType::Big;
		this->elapsedTime = LUIGI_SDELAYTIME / 10.0f;
		this->sDelayTime = LUIGI_SDELAYTIME;
		PlaySound(this->pGrowUpToBigSound);
	}
}

void CLuigi::GrowUpToFire()
{
	if (this->type != LuigiType::Fire)
	{
		if (this->type == LuigiType::Small)
		{
			switch (this->moveType)
			{
			case LuigiMoveType::Standing:
			{

			}
			break;

			case LuigiMoveType::Running:
			{
				if (this->index > 0 && this->index < 3)
					this->index++;
				else
					this->index += 2;
			}
			break;

			case LuigiMoveType::Jumping:
			{

			}
			break;
			}
		}

		this->state = LuigiState::GrowToFire;
		this->type = LuigiType::Big;
		this->elapsedTime = LUIGI_SDELAYTIME / 10.0f;
		this->sDelayTime = LUIGI_SDELAYTIME;
		PlaySound(this->pGrowUpToFireSound);
	}
}

void CLuigi::ShrinkToSmall()
{
	if (this->type != LuigiType::Small)
	{
		this->state = LuigiState::Shink;
		this->type = LuigiType::Big;
		this->elapsedTime = LUIGI_SDELAYTIME / 10.0f;
		this->sDelayTime = LUIGI_SDELAYTIME;
		PlaySound(this->pGrowUpToBigSound);
	}
}

void CLuigi::GoToHeaven()
{
	this->type = LuigiType::Small;
	this->state = LuigiState::Die;
	this->elapsedTime = LUIGI_DELAYTIME1;
	this->velocity = { 0,LUIGI_JUMPVELOCITYY2 };
	PlaySound(this->pDieSound);
}

void CLuigi::test()
{
	this->hue = 0.0f;
	this->invincibleTime = LUIGI_INVINCIBLETIME;
}

BoudingBox CLuigi::getBox()
{
	float height;

	if (this->type == LuigiType::Small)
		height = LUIGI_HEIGHTSIZE;
	else
		this->frame == 1 ?
		height = LUIGI_HEIGHTSIZE : height = BIGLUIGI_HEIGHTSIZE;

	return BoudingBox(
		this->_position.x - LUIGI_WIDTHSIZE / 2.0f,
		this->_position.y + height,
		LUIGI_WIDTHSIZE, height,
		this->velocity.x, this->velocity.y);
}

void CLuigi::StopJump()
{
	if (this->moveType == LuigiMoveType::Jumping)
	{
		this->moveType = LuigiMoveType::Running;
		this->elapsedTime = this->delayTime;
		if (this->fDelayTime != 0.0f)
			this->fDelayTime = 0.0f;
		this->_scale.x = this->direction*std::abs(this->_scale.x);
	}
}