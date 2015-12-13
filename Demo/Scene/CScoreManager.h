#pragma once

#include "CScore.h"
#include "CGameplayPanel.h"
#include "../Camera.h"
#include <vector>
#include <map>

struct TScoreBonus
{
	int score;
	int nextScore;
	float timeScoreBonus;

	TScoreBonus():
		score(0),
		nextScore(0),
		timeScoreBonus(0) {}


	TScoreBonus(int currentScore,int nextScore,float timeScoreBonus):
		score(currentScore),
		nextScore(nextScore),
		timeScoreBonus(timeScoreBonus) {}
};

class CScoreManager
{
private:
	static IDirect3DTexture9* pTexture;
	static std::map<int, RECT> listSrcRect;

	static std::vector<CScore> listScore;

	static TScoreBonus currentScore;
	static float timeScoreBonus;
	static std::map<int, TScoreBonus> listScoreBonus;

public:
	static void Initialize(IDirect3DDevice9* pD3DDevice);
	static void Destroy();

	static void Update(float elapsedTime);
	static void Render(ID3DXSprite* pSprite, CCamera* const pCamera);

	static void AddScore(D3DXVECTOR2 position);
};