#pragma once

#include "CEnemy.h"

class CEnemy0 :public CEnemy
{
private:
	static IDirect3DTexture9* texture;

	void Update_Velocity(float elapsedTime);
	void Update_Collision(float elapsedTime, CMap* const pMap,
		std::map<int, CEnemy*> listEnemy);
	void Update_SourceRect(float elapsedTime);

	void CollisionLine(float elapsedTime,
		std::vector<std::pair<D3DXVECTOR2, D3DXVECTOR2>>* const listLine);
	void CollisionLineX(float elapsedTime, float collisionTime,
		D3DXVECTOR2 point1, D3DXVECTOR2 point2, float normalX, float normalY);
	void CollisionLineY(float elapsedTime, float collisionTime,
		D3DXVECTOR2 point1, float normalX, float normalY);

	void CollisionEnemy(float elapsedTime,
		std::map<int, CEnemy*> listEnemy);

	float GetWSrcRect() { return float(this->sourceRect.right - this->sourceRect.left); }
	float GetHSrcRect() { return float(this->sourceRect.bottom - this->sourceRect.top); }

public:
	static void InitializeTexture(IDirect3DDevice9* pD3DDevice);
	static void ReleaseTexture();

	CEnemy0(EnemyInfo info);

	void Update(float elapsedTime, CMap* const pMap,
		std::map<int, CEnemy*> listEnemy);
	void Render(ID3DXSprite* pSprite, CCamera* const pCamera);

	BoudingBox GetBox();
};