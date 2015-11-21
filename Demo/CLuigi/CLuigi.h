#pragma once

#include "../CObject.h"
#include "../DX/CDXInput.h"

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

	void Update_Normal(float elapsedTime, CDXInput* inputDevice);
	void Update_Normal_GhostTime(float elapsedTime);
	void Update_Normal_InvincibleTime(float elapsedTime);
	void Update_Normal_MoveType(CDXInput* inputDevice);
	void Update_Normal_Velocity(float elapsedTime, CDXInput* inputDevice);
	void Update_Normal_DelayTime(float elapsedTime, CDXInput* inputDevice);
	void Update_Normal_Frame(float elapsedTime, CDXInput* inputDevice);
	void Update_Normal_SourceRect();
	void Update_Normal_Scale();

	void IncreaseVelocityX(float elapsedTime, float limit);
	void DecreaseVelocityX(float elapsedTime, float k);
	void IncreaseVelocityY(float elapsedTime, float k);

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

public:
	CLuigi() {}
	~CLuigi();

	void Initialize(IDirect3DDevice9* pD3DDevice);
	void Update(float elapsedTime, CDXInput* inputDevice);
	void Render(ID3DXSprite* pSprite);

	void OnLostDevice();
	void OnResetDevice();

	void GrowUpToBig();
	void GrowUpToFire();
	void ShrinkToSmall();
	void GoToHeaven();
	void test();

	int GetPosition() { return this->state; }
	int GetState() { return this->state; }
	int GetMoveType() { return this->moveType; }
	float GetDelayTime() { return this->delayTime; }
	int GetDirection() { return this->direction; }
	D3DXVECTOR2 GetVelocity() { return this->velocity; }
};