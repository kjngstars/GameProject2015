#include "CScore.h"

#define TTL 50.0f

CScore::CScore(int score, D3DXVECTOR2 position)
{
	this->score = score;
	this->position = position;
	this->ttl = TTL;
}

void CScore::Update(float elapsedTime)
{
	this->position.y += 1;

	if (this->ttl > 0)
	{
		this->ttl--;

		if (this->ttl < 0)
			this->ttl = 0;
	}

	this->color.a = this->ttl / TTL;
}