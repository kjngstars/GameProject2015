#pragma once

#include <d3dx9.h>
#include <vector>
#include "../BoudingBox.h" 
#include "../CMap.h"

class CLuigiBullet
{
private:
	D3DXVECTOR2 position;
	D3DXVECTOR2 velocity;
	RECT sourceRect;
	float rotate;
	float elapsedTime;
	float ttl;

public:
	CLuigiBullet(D3DXVECTOR2 position, float direction);

	void Update(float elapsedTime, CMap* const pMap);

	BoudingBox GetBox();

	friend class CLuigiBulletManager;
};

static class CLuigiBulletManager
{
private:
	static IDirect3DTexture9* bulletTexture;
	static std::vector<CLuigiBullet> listBullet;

public:
	static void Initialize(IDirect3DDevice9* pD3DDevice);
	static void Destroy();

	static void Update(float elapsedTime, CCamera* const pCamera, CMap* const pMap);
	static void Render(ID3DXSprite* pSprite, CCamera* const pCamera);

	static bool Shoot(D3DXVECTOR2 position, int direction);

	static std::vector<CLuigiBullet> GetListBullet() { return listBullet; }
};