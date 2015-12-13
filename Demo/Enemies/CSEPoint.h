#pragma once

#include <d3dx9.h>
#include <vector>
#include "../Camera.h"

class CSEPoint
{
private:
	D3DXVECTOR2 position;
	D3DXVECTOR2 velocity;
	D3DXVECTOR2 scale;
	float rotate;
	float elapsedTime;
	float ttl;
	D3DXCOLOR color = D3DCOLOR_ARGB(255, 255, 255, 255);

public:
	CSEPoint(D3DXVECTOR2 position);

	void Update(float elapsedTime);

	float GetPercentageTime()
	{
		return this->elapsedTime / this->ttl;
	}

	friend class CSEPointManager;
};

class CSEPointManager
{
private:
	static IDirect3DTexture9* pointTexture;
	static std::vector<CSEPoint> listPoint;

public:
	static void Initialize(IDirect3DDevice9* pD3DDevice);
	static void Destroy();

	static void Update(float elapsedTime);
	static void Render(ID3DXSprite* pSprite, CCamera* const pCamera);

	static void AddPoint(D3DXVECTOR2 position);
};