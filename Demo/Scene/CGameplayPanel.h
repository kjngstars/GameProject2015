#pragma once

#include <d3dx9.h>
#include <string>
#include <map>
#include "../DX/dxaudio.h"

class CGameplayPanel
{
private:
	static float time;
	static int life;
	static int coin;
	static int totalScored;
	static IDirect3DTexture9* pBackground;
	static IDirect3DTexture9* pNumber0;
	static IDirect3DTexture9* pNumber1;
	static std::map<int, RECT> listSrcRectNumber;
	static CSound* pBonusLifeSound;

	static void DrawTime(ID3DXSprite* pSprite);
	static void DrawNumber0(int number, D3DXVECTOR2 position, ID3DXSprite* pSprite);// top right

public:
	static void Initialize(IDirect3DDevice9* pD3DDevice);
	static void Destroy();

	static void Update(float elapsedTime);
	static void Render(ID3DXSprite* pSprite);

	static float GetTime() { return CGameplayPanel::time; };
	static void BonusLife();
	static void Reset(ID3DXSprite* pSprite);

	friend class CScoreManager;
};