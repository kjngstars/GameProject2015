#pragma once

#include "../CObject.h"
#include "../DX/CDXInput.h"
#include "../Camera.h"
#include "../CMap.h"
#include "CLuigiBullet.h"
#include "../DX/dxaudio.h"

extern enum LuigiType;
extern enum LuigiState;
extern enum LuigiMoveType;

class CLuigi :public CObject
{
private:
	LuigiType type;
	LuigiState state;
	LuigiMoveType moveType;

	ID3DXEffect* pEffect = 0;

	float delayTime;
	float elapsedTime;
	float ghostTime;
	float invincibleTime;
	float fDelayTime;
	float sDelayTime;

	int index;
	int frame;

	RECT sourceRect;
	D3DXCOLOR color;

	IDirect3DTexture9* luigiCloth = 0;
	float hue;

	int direction = 0;
	D3DXVECTOR2 velocity;

#pragma region Sound
	CSound* pJumpSound = 0;
	CSound* pGrowUpToBigSound = 0;
	CSound* pGrowUpToFireSound = 0;
	CSound* pFireSound = 0;
	CSound* pDieSound = 0;
#pragma endregion

	void Update_Normal(float elapsedTime, CDXInput* const inputDevice, CMap* const pMap);
	void Update_Normal_GhostTime(float elapsedTime);
	void Update_Normal_InvincibleTime(float elapsedTime);
	void Update_Normal_MoveType(float elapsedTime, CDXInput* const inputDevice);
	void Update_Normal_Velocity(float elapsedTime, CDXInput* const inputDevice);
	void Update_Normal_Collision(float elapsedTime, CMap* const pMap);
	void Update_Normal_DelayTime(float elapsedTime, CDXInput* const inputDevice);
	void Update_Normal_Frame(float elapsedTime, CDXInput* const inputDevice);
	void Update_Normal_SourceRect();
	void Update_Normal_Scale();

	void IncreaseVelocityX(float elapsedTime, float limit);
	void DecreaseVelocityX(float elapsedTime, float k);
	void DecreaseVelocityY(float elapsedTime, float k);

	void CollisionLine(float elapsedTime,
		std::vector<std::pair<D3DXVECTOR2, D3DXVECTOR2>>* const listLine);
	void CollisionLineX(float elapsedTime, float collisionTime,
		D3DXVECTOR2 point1, D3DXVECTOR2 point2, float normalX, float normalY);
	void CollisionLineY(float elapsedTime, float collisionTime,
		D3DXVECTOR2 point1, float normalX, float normalY);
	bool CollisionEnemy();

	void Update_GrowToBig(float elapsedTime);
	void Update_GrowToFire(float elapsedTime);
	void Update_ShrinkToSmall(float elapsedTime);
	void Update_Die(float elapsedTime);

	void Render_Normal(ID3DXSprite* pSprite);
	void Render_GrowToBig(ID3DXSprite* pSprite);
	void Render_GrowToFire(ID3DXSprite* pSprite);
	void Render_ShrinkToSmall(ID3DXSprite* pSprite);
	void Render_Die(ID3DXSprite* pSprite);

	float GetWSrcRect() { return float(this->sourceRect.right - this->sourceRect.left); }
	float GetHSrcRect() { return float(this->sourceRect.bottom - this->sourceRect.top); }

	void StopJump();

public:
	CLuigi() {}
	~CLuigi();

	void Initialize(IDirect3DDevice9* pD3DDevice);
	void Update(float elapsedTime, CDXInput* const inputDevice, CMap* const pMap);
	void Render(ID3DXSprite* pSprite, CCamera* const pCamera);

	void OnLostDevice();
	void OnResetDevice();

	void GrowUpToBig();
	void GrowUpToFire();
	void ShrinkToSmall();
	void GoToHeaven();
	void test();

	BoudingBox getBox();

	bool IsNormalState();

	D3DXVECTOR2 GetPosition() { return this->_position; }
	int GetState() { return this->state; }
	int GetMoveType() { return this->moveType; }
	float GetDelayTime() { return this->delayTime; }
	int GetDirection() { return this->direction; }
	D3DXVECTOR2 GetVelocity() { return this->velocity; }
};