#include "CSPoint.h"
#include "LuigiConstant.h"
#include "../GameConfig.h"
#include <stdlib.h>
#include <time.h>

#define SPOINTMANAGERDELAYTIME 100.0f

static const D3DXVECTOR2 SPointAnchorPoint =
D3DXVECTOR2(32.0f, 32.0f);

IDirect3DTexture9* CSPointManager::pointTexture = 0;
std::vector<CSPoint> CSPointManager::listPoint;
float CSPointManager::elapsedTime = 0.0f;

static float GetRandomFloat(float min, float max)
{
	return min + (float)rand() / RAND_MAX*
		(max - min);
}

CSPoint::CSPoint(D3DXVECTOR2 luigiPosition, float luigiHeight)
{
	this->position.x = luigiPosition.x +
		GetRandomFloat(-32.0f, 32.0f);
	this->position.y = luigiPosition.y+
		GetRandomFloat(0.0f, luigiHeight);
	this->scale.x = this->scale.y =
		GetRandomFloat(0.0f, 1.0f);
	this->rotate = GetRandomFloat(0.0f, D3DX_PI);
	this->elapsedTime = 0.0f;
	this->ttl = GetRandomFloat(500.0f, 3000.0f);
	this->velocity.x = 0.0f;
	this->velocity.y = GetRandomFloat(luigiHeight / 2.0f, luigiHeight*2.0f) /
		this->ttl;
}

void CSPoint::Update(float elapsedTime)
{
	this->position += this->velocity*elapsedTime;
	this->rotate -= 0.05;
	this->elapsedTime += elapsedTime;
	this->scale.x = this->scale.y = GetRandomFloat(0.0f, 1.0f);
}

void CSPointManager::Initialize(IDirect3DDevice9* pD3DDevice)
{
	if (CSPointManager::pointTexture != nullptr)
		CSPointManager::pointTexture->Release();

	D3DXIMAGE_INFO info;
	HR(D3DXGetImageInfoFromFile("./Content/Luigi/SPoint.png", &info));
	HR(D3DXCreateTextureFromFileEx(
		pD3DDevice, "./Content/Luigi/SPoint.png",
		info.Width, info.Height, D3DX_DEFAULT, 0,
		D3DFMT_UNKNOWN, D3DPOOL_MANAGED,
		D3DX_DEFAULT, D3DX_DEFAULT,
		0, &info, 0, &CSPointManager::pointTexture));
}

void CSPointManager::Destroy()
{
	SAFE_RELEASE(CSPointManager::pointTexture);
	CSPointManager::listPoint.clear();
}

void CSPointManager::Update(float elapsedTime)
{
	srand(GetTickCount());

	for (int i = 0;i < CSPointManager::listPoint.size();i++)
	{
		CSPointManager::listPoint.at(i).Update(elapsedTime);

		if (CSPointManager::listPoint.at(i).elapsedTime >=
			CSPointManager::listPoint.at(i).ttl)
		{
			CSPointManager::listPoint.erase(CSPointManager::listPoint.begin() + i);
			i--;
		}
	}
}

void CSPointManager::Render(ID3DXSprite* pSprite, CCamera* const pCamera)
{
	for (int i = 0;i < CSPointManager::listPoint.size();i++)
	{
		MATRIX matTransform;

		D3DXVECTOR2 position_temp = pCamera->getPointTransform(
			CSPointManager::listPoint.at(i).position.x,
			CSPointManager::listPoint.at(i).position.y);

		D3DXMatrixTransformation2D(
			&matTransform,
			&SPointAnchorPoint, 0,
			&(CSPointManager::listPoint.at(i).scale*
				(1 - CSPointManager::listPoint.at(i).GetPercentageTime())),
			&SPointAnchorPoint, CSPointManager::listPoint.at(i).rotate,
			&D3DXVECTOR2(
				position_temp.x - SPointAnchorPoint.x,
				position_temp.y - SPointAnchorPoint.y));
		pSprite->SetTransform(&matTransform);

		pSprite->Draw(
			CSPointManager::pointTexture,
			0, 0, 0, D3DCOLOR_XRGB(255, 255, 255));
	}
}

void CSPointManager::AddPoint(float elapsedTime, D3DXVECTOR2 luigiPosition, float luigiHeight)
{
	CSPointManager::elapsedTime += elapsedTime;
	if (CSPointManager::elapsedTime > SPOINTMANAGERDELAYTIME)
	{
		srand(GetTickCount());

		for (int i = 0;i < 5;i++)
			CSPointManager::listPoint.push_back(
				CSPoint(luigiPosition, luigiHeight));

		CSPointManager::elapsedTime = 0.0f;
	}
}