#pragma once

#include "CEnemy.h"

class CEnemy1 :public CEnemy
{
private:
	static IDirect3DTexture9* texture;

	bool checkMove = true;	// xác định enemy có nhúc nhích không

	void Update_Velocity(float elapsedTime);
	void Update_Collision(float elapsedTime, CMap* const pMap,
		CCamera* const pCamera, std::map<int, CEnemy*> listEnemyAlive);
	void Update_SourceRect(float elapsedTime);

	void CollisionLine(float elapsedTime,
		std::vector<std::pair<D3DXVECTOR2, D3DXVECTOR2>>* const listLine);
	void CollisionLineX(float elapsedTime, float collisionTime,
		D3DXVECTOR2 point1, D3DXVECTOR2 point2, float normalX, float normalY);
	void CollisionLineY(float elapsedTime, float collisionTime,
		D3DXVECTOR2 point1, float normalX, float normalY);

	void CollisionEnemy(float elapsedTime,
		std::map<int, CEnemy*> listEnemyAlive);

	float GetWSrcRect() { return float(this->sourceRect.right - this->sourceRect.left); }
	float GetHSrcRect() { return float(this->sourceRect.bottom - this->sourceRect.top); }

public:
	static void InitializeTexture(IDirect3DDevice9* pD3DDevice);
	static void ReleaseTexture();

	CEnemy1(EnemyInfo info);

	void Update(float elapsedTime, CMap* const pMap,
		CCamera* const pCamera, std::map<int, CEnemy*> listEnemyAlive);
	void Render(ID3DXSprite* pSprite, CCamera* const pCamera);

	BoudingBox GetBox();
	bool IsViolent();

	void DescreaseHP();
	void TrampledToDeath();
	void BulletHit() {} // dính ??n không xi nhê

	bool Dame(float sign);
};