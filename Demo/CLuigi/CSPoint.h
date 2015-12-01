#pragma once

#include <d3dx9.h>
#include <vector>
#include "../Camera.h"

class CSPoint
{
private:
	D3DXVECTOR2 position;
	D3DXVECTOR2 velocity;
	D3DXVECTOR2 scale;
	float rotate;
	float elapsedTime;
	float ttl;

	float GetPercentageTime()
	{
		return this->elapsedTime / this->ttl;
	}

public:
	CSPoint(D3DXVECTOR2 luigiPosition, float luigiHeight);

	void Update(float elapsedTime);

	friend class CSPointManager;
};

static class CSPointManager
{
private:
	static IDirect3DTexture9* pointTexture;
	static std::vector<CSPoint> listPoint;
	static float elapsedTime;

public:
	static void Initialize(IDirect3DDevice9* pD3DDevice);
	static void Destroy();

	static void Update(float elapsedTime);
	static void Render(ID3DXSprite* pSprite, CCamera* const pCamera);

	static void AddPoint(float elapsedTime, D3DXVECTOR2 luigiPosition, float luigiHeight);
};