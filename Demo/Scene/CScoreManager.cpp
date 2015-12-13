#include "CScoreManager.h"
#include "../GameConfig.h"
#include <fstream>
#include <sstream>

#define SRCRECTHEIGHT 16
#define MINSCORE 200

static const D3DXVECTOR2 ScoreAnchorPoint = { 24.0f,8.0f };

IDirect3DTexture9* CScoreManager::pTexture = 0;
std::map<int, RECT> CScoreManager::listSrcRect;
std::vector<CScore> CScoreManager::listScore;

TScoreBonus CScoreManager::currentScore;
float CScoreManager::timeScoreBonus = 0;
std::map<int, TScoreBonus> CScoreManager::listScoreBonus;

void CScoreManager::Initialize(IDirect3DDevice9* pD3DDevice)
{
	if (CScoreManager::pTexture != nullptr)
		CScoreManager::pTexture->Release();

	D3DXIMAGE_INFO info;
	HR(D3DXGetImageInfoFromFile("./Content/Scene/Score.png", &info));
	HR(D3DXCreateTextureFromFileEx(
		pD3DDevice, "./Content/Scene/Score.png",
		info.Width, info.Height, D3DX_DEFAULT, 0,
		D3DFMT_UNKNOWN, D3DPOOL_MANAGED,
		D3DX_DEFAULT, D3DX_DEFAULT,
		0, &info, 0, &CScoreManager::pTexture));

	std::ifstream file("./Content/Scene/Score.txt");
	std::string line;
	int score;

	for (int i = 0;i < 8;i++)
	{
		std::getline(file, line);
		std::istringstream iss(line);
		iss >> score;

		CScoreManager::listSrcRect[score].left = 0;
		CScoreManager::listSrcRect[score].right = info.Width + 1;
		CScoreManager::listSrcRect[score].top = i*SRCRECTHEIGHT;
		CScoreManager::listSrcRect[score].bottom =
			CScoreManager::listSrcRect[score].top + SRCRECTHEIGHT + 1;
	}

	file.close();

	file.open("./Content/Scene/ScoreBonus.txt");

	int nextScore;
	float timeScoreBonus;

	for (int i = 0;i < 8;i++)
	{
		std::getline(file, line);
		std::istringstream iss(line);

		iss >> score;
		iss >> nextScore;
		iss >> timeScoreBonus;

		CScoreManager::listScoreBonus[score] =
			TScoreBonus(score, nextScore, timeScoreBonus);
	}

	file.close();

	CScoreManager::currentScore = CScoreManager::listScoreBonus[MINSCORE];
	CScoreManager::timeScoreBonus = 0;
}

void CScoreManager::Destroy()
{
	SAFE_RELEASE(CScoreManager::pTexture);
	CScoreManager::listScore.clear();
}

void CScoreManager::Update(float elapsedTime)
{
	for (int i = 0;i < CScoreManager::listScore.size();i++)
	{
		CScoreManager::listScore.at(i).Update(elapsedTime);

		if (CScoreManager::listScore.at(i).ttl <= 0.0f)
		{
			CScoreManager::listScore.erase(
				CScoreManager::listScore.begin() + i);
			i--;
		}
	}

	if (CScoreManager::timeScoreBonus > 0.0f)
	{
		CScoreManager::timeScoreBonus -= elapsedTime;

		if (CScoreManager::timeScoreBonus < 0.0f)
			CScoreManager::timeScoreBonus = 0;
	}
}

void CScoreManager::Render(ID3DXSprite* pSprite, CCamera* const pCamera)
{
	for (int i = 0;i < CScoreManager::listScore.size();i++)
	{
		MATRIX matTransform;

		D3DXVECTOR2 position_temp = pCamera->getPointTransform(
			CScoreManager::listScore.at(i).position.x,
			CScoreManager::listScore.at(i).position.y);

		D3DXMatrixTransformation2D(
			&matTransform,
			&ScoreAnchorPoint, 0,
			&D3DXVECTOR2(1.0f, 1.0f),
			&ScoreAnchorPoint, 0,
			&D3DXVECTOR2(
				position_temp.x - ScoreAnchorPoint.x,
				position_temp.y - ScoreAnchorPoint.y));
		pSprite->SetTransform(&matTransform);

		RECT srcRect = CScoreManager::listSrcRect[CScoreManager::listScore.at(i).score];

		pSprite->Draw(
			CScoreManager::pTexture,
			&srcRect,
			0, 0, CScoreManager::listScore.at(i).color);
	}
}

void CScoreManager::AddScore(D3DXVECTOR2 position)
{
	if (CScoreManager::timeScoreBonus <= 0)
		CScoreManager::currentScore = CScoreManager::listScoreBonus[MINSCORE];

	CGameplayPanel::totalScored += CScoreManager::currentScore.score;

	CScoreManager::listScore.push_back(
		CScore(CScoreManager::currentScore.score, position));

	if (CScoreManager::currentScore.score == 0)
		CGameplayPanel::BonusLife();

	CScoreManager::timeScoreBonus = currentScore.timeScoreBonus;

	CScoreManager::currentScore =
		CScoreManager::listScoreBonus[CScoreManager::currentScore.nextScore];
}