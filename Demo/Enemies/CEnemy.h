#pragma once

#include "../CObject.h"
#include "../CMap.h"
#include "../Scene/CGameplayPanel.h"

#define DIEFLAG_GOODBYE -1	// tạm biệt baby
#define DIEFLAG_NONE 0		// chưa ngủm
#define DIEFLAG_TRAMPLE 1	// bị chà đạp mà ngủm
#define DIEFLAG_BULLET 2	// trúng đạn ngủm
#define DIEFLAG_UNKNOWN 3	// ngủm không biết lý do

class CEnemy
{
protected:
	int id = 0;
	int type = 0;
	D3DXVECTOR2 position = { 0.0f,0.0f };

	D3DXVECTOR2 scale = { 1.0f,1.0f };

	int direction = 1;
	D3DXVECTOR2 velocity = { 0.0f, 0.0f };

	float elapsedTime = 0.0f;

	int hp = 0;
	int dieFlag = DIEFLAG_NONE;

	RECT patrolRect;
	RECT sourceRect;

	int score = 1;

public:
	virtual void Update(float elapsedTime, CMap* const pMap,
		CCamera* const pCamera, std::map<int, CEnemy*> listEnemyAlive) {};
	virtual void Render(ID3DXSprite* pSprite, CCamera* const pCamera) {};

	virtual bool IsDied() { return this->dieFlag!=DIEFLAG_NONE; };
	virtual bool IsEnable() { return this->dieFlag != DIEFLAG_GOODBYE; };

	virtual BoudingBox GetBox() { return BoudingBox(); };
	virtual D3DXVECTOR2 GetPosition() { return this->position; }
	virtual int GetID() { return this->id; }
	virtual int GetType() { return this->type; }
	virtual D3DXVECTOR2 GetVelocity() { return this->velocity; }
	virtual int GetHP() { return this->hp; }
	virtual bool IsViolent() { return false; }	// đang giận
	virtual int Score() { return this->score; }

	virtual void DescreaseHP();
	virtual void TrampledToDeath();				// bị đạp đầu chết
	virtual void BulletHit();					// bị bắn 
	virtual void Die(float sign);				// bị ? chết

	virtual bool Dame(float sign);

	friend class CEnemiesManager;
};