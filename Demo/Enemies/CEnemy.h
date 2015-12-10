#pragma once

#include "../CObject.h"
#include "../CMap.h"

class CEnemy
{
protected:
	int id = 0;
	D3DXVECTOR2 position = { 0.0f,0.0f };

	D3DXVECTOR2 scale = { 1.0f,1.0f };

	int direction = 1;
	D3DXVECTOR2 velocity = { 0.0f, 0.0f };

	float elapsedTime = 0.0f;

	int hp = 0;
	int isDied = 0;

	RECT patrolRect;
	RECT sourceRect;

public:
	virtual void Update(float elapsedTime, CMap* const pMap,
		CCamera* const pCamera, std::map<int, CEnemy*> listEnemyAlive) {};
	virtual void Render(ID3DXSprite* pSprite, CCamera* const pCamera) {};

	virtual BoudingBox GetBox() { return BoudingBox(); };

	virtual D3DXVECTOR2 GetPosition() { return this->position; }
	virtual int GetID() { return this->id; }

	virtual void DescreaseHP();
	virtual void Die1();	// bị đạp đầu chết
	virtual void Die2();	// bị bắn chết
	virtual void Die3(float sign);	// bị ? chết

	friend class CEnemiesManager;
};