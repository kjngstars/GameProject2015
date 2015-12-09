#include "CSEPoint.h"
#include <stdlib.h>
#include <time.h>

static const D3DXVECTOR2 SEPointAnchorPoint =
D3DXVECTOR2(16.0f, 16.0f);

IDirect3DTexture9* CSEPointManager::pointTexture = 0;
std::vector<CSEPoint> CSEPointManager::listPoint;

static float GetRandomFloat(float min, float max)
{
	return min + (float)rand() / RAND_MAX*
		(max - min);
}

CSEPoint::CSEPoint(D3DXVECTOR2 position)
{
	this->position = position;
	this->velocity.x = GetRandomFloat(-0.1f, 0.1f);
	this->velocity.y = GetRandomFloat(-0.1f, 0.1f);
	this->scale.x = this->scale.y =
		GetRandomFloat(0.0f, 0.3f);
	this->rotate = GetRandomFloat(0.0f, D3DX_PI);
	this->elapsedTime = 0.0f;
	this->ttl = GetRandomFloat(500.0f, 1000.0f);
}

void CSEPoint::Update(float elapsedTime)
{
	this->position += this->velocity*elapsedTime;
	this->velocity.x > 0.0f ?
		this->rotate -= 0.05 :
		this->rotate += 0.05;
	this->elapsedTime += elapsedTime;
	this->scale.x = this->scale.y += 0.005;
	this->color.a = 1.0f - this->GetPercentageTime()/2.0f;
}

void CSEPointManager::Initialize(IDirect3DDevice9* pD3DDevice)
{
	if (CSEPointManager::pointTexture != nullptr)
		CSEPointManager::pointTexture->Release();

	D3DXIMAGE_INFO info;
	HR(D3DXGetImageInfoFromFile("./Content/Enemies/Star.png", &info));
	HR(D3DXCreateTextureFromFileEx(
		pD3DDevice, "./Content/Enemies/Star.png",
		info.Width, info.Height, D3DX_DEFAULT, 0,
		D3DFMT_UNKNOWN, D3DPOOL_MANAGED,
		D3DX_DEFAULT, D3DX_DEFAULT,
		0, &info, 0, &CSEPointManager::pointTexture));
}

void CSEPointManager::Destroy()
{
	SAFE_RELEASE(CSEPointManager::pointTexture);
}

void CSEPointManager::Update(float elapsedTime)
{
	for (int i = 0;i < CSEPointManager::listPoint.size();i++)
	{
		CSEPointManager::listPoint.at(i).Update(elapsedTime);

		if (CSEPointManager::listPoint.at(i).elapsedTime >=
			CSEPointManager::listPoint.at(i).ttl)
		{
			CSEPointManager::listPoint.erase(CSEPointManager::listPoint.begin() + i);
			i--;
		}
	}
}

void CSEPointManager::Render(ID3DXSprite* pSprite, CCamera* const pCamera)
{
	for (int i = 0;i < CSEPointManager::listPoint.size();i++)
	{
		MATRIX matTransform;

		D3DXVECTOR2 position_temp = pCamera->getPointTransform(
			CSEPointManager::listPoint.at(i).position.x,
			CSEPointManager::listPoint.at(i).position.y);

		D3DXMatrixTransformation2D(
			&matTransform,
			&SEPointAnchorPoint, 0,
			&CSEPointManager::listPoint.at(i).scale,
			&SEPointAnchorPoint, CSEPointManager::listPoint.at(i).rotate,
			&D3DXVECTOR2(
				position_temp.x - SEPointAnchorPoint.x,
				position_temp.y - SEPointAnchorPoint.y));
		pSprite->SetTransform(&matTransform);

		pSprite->Draw(
			CSEPointManager::pointTexture,
			0, 0, 0, CSEPointManager::listPoint.at(i).color);
	}
}

void CSEPointManager::AddPoint(D3DXVECTOR2 position)
{
	srand(GetTickCount());

	for (int i = 0;i < 20;i++)
		CSEPointManager::listPoint.push_back(
			CSEPoint(position));
}