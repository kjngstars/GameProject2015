#pragma once

#include <d3dx9.h>

class CScore
{
private:
	int score = 0;
	D3DXVECTOR2 position = { 0.0f,0.0f };
	D3DXCOLOR color = D3DCOLOR_XRGB(255, 255, 255);
	float ttl = 0;

public:
	CScore(int score, D3DXVECTOR2 position);

	void Update(float elapsedTime);

	friend class CScoreManager;
};