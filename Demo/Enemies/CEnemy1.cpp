#include "CEnemy1.h"
#include "../GameConfig.h"
#include "../Collision/Collision.h"
#include "CEnemiesManager.h"

#define ENEMY1_WIDTH 34.0f
#define ENEMY1_HEIGHT 24.0f

#define ENEMY1_WSRCRECT 34.0f
#define ENEMY1_HSRCRECT 34.0f

#define ENEMY1_LIMITVELOCITYX0 0.08f
#define ENEMY1_LIMITVELOCITYX1 0.48f

#define ENEMY1_DACCELERATIONY 0.0016f
#define ENEMY1_LIMITVELOCITYY -0.64f

#define ENEMY1_DELAYTIME0 50.0f
#define ENEMY1_DELAYTIME1 100.0f

#define ENEMY1_MAXHP 2

IDirect3DTexture9* CEnemy1::texture = 0;

void CEnemy1::InitializeTexture(IDirect3DDevice9* pD3DDevice)
{
	if (CEnemy1::texture != nullptr)
		CEnemy1::texture->Release();

	D3DXIMAGE_INFO info;
	HR(D3DXGetImageInfoFromFile("./Content/Enemies/Enemy1.png", &info));
	HR(D3DXCreateTextureFromFileEx(
		pD3DDevice, "./Content/Enemies/Enemy1.png",
		info.Width, info.Height, D3DX_DEFAULT, 0,
		D3DFMT_UNKNOWN, D3DPOOL_MANAGED,
		D3DX_DEFAULT, D3DX_DEFAULT,
		0, &info, 0, &CEnemy1::texture));
}

void CEnemy1::ReleaseTexture()
{
	SAFE_RELEASE(CEnemy1::texture)
}

CEnemy1::CEnemy1(EnemyInfo info)
{
	this->id = info._id;

	this->position.x = info._x + ENEMY1_WSRCRECT / 2.0f;
	this->position.y = info._y;

	if (info._direction == 1)
	{
		this->scale.x = this->direction = -1;
		this->patrolRect.left = info._x - info._patrolWidth;
	}
	else
	{
		this->scale.x = this->direction = 1;
		this->patrolRect.left = info._x;
	}
	this->patrolRect.top = info._y;
	this->patrolRect.right = this->patrolRect.left + info._patrolWidth;
	this->patrolRect.bottom = this->patrolRect.top + info._patrolHeight;

	this->velocity.x = this->direction*ENEMY1_LIMITVELOCITYX0;
	this->velocity.y = ENEMY1_LIMITVELOCITYY;

	this->sourceRect = { 0,0,ENEMY1_WSRCRECT ,ENEMY1_HSRCRECT };

	this->hp = ENEMY1_MAXHP;
}

BoudingBox CEnemy1::GetBox()
{
	return BoudingBox(
		this->position.x - ENEMY1_WIDTH / 2.0f,
		this->position.y + ENEMY1_HEIGHT,
		ENEMY1_WIDTH, ENEMY1_HEIGHT,
		this->velocity.x,
		this->velocity.y);
}

bool CEnemy1::IsViolent()
{
	return this->hp == 1 && this->checkMove;
}

void CEnemy1::DescreaseHP()
{
	if (this->hp > 1)
	{	//bị đạp
		this->hp--;

		this->velocity.x = 0.0f;
		this->checkMove = false;	//ngưng nhúc nhích
	}
	else if (this->hp == 1)
		this->checkMove = !this->checkMove;
}

void CEnemy1::TrampledToDeath()
{
	this->DescreaseHP();
}

bool CEnemy1::Dame(float sign)
{//	lúc luigi đụng zô
	if (this->hp == 1 &&
		!this->checkMove)
	{// hp=1 và không nhúc nhích
		this->checkMove = true; //cho nhúc nhích

		sign < 0 ?	// gán direction cùng dấu vận tốc luigi
			this->direction = -1 : this->direction = 1;

		CEnemiesManager::Playsound();

		return false;
	}

	return true;
}

void CEnemy1::Update(float elapsedTime, CMap* const pMap,
	CCamera* const pCamera, std::map<int, CEnemy*> listEnemyAlive)
{
	if (this->IsEnable())
	{
		this->Update_Velocity(elapsedTime);
		this->Update_Collision(elapsedTime, pMap, pCamera, listEnemyAlive);
		this->Update_SourceRect(elapsedTime);
	}
}

void CEnemy1::Update_Velocity(float elapsedTime)
{
	if (this->hp > 1)
	{// hp>1 úp đát bình thường
		if (this->velocity.x*this->direction != ENEMY1_LIMITVELOCITYX0)
			this->velocity.x = this->direction*ENEMY1_LIMITVELOCITYX0;
	}
	else if (this->hp == 1)
	{// hp=1 kiểm tra cờ nhúc nhích
		this->checkMove ?
			this->velocity.x = this->direction*ENEMY1_LIMITVELOCITYX1 : // nhúc nhích
			this->velocity.x = 0.0f;									// không nhúc nhích
	}

	if (this->velocity.y > ENEMY1_LIMITVELOCITYY)
	{
		this->velocity.y -= ENEMY1_DACCELERATIONY*elapsedTime;

		if (this->velocity.y < ENEMY1_LIMITVELOCITYY)
			this->velocity.y = ENEMY1_LIMITVELOCITYY;
	}
}

void CEnemy1::Update_Collision(float elapsedTime, CMap* const pMap,
	CCamera* const pCamera, std::map<int, CEnemy*> listEnemyAlive)
{
	switch (this->dieFlag)
	{
	case DIEFLAG_NONE:	//chưa ngủm
	{
		this->CollisionEnemy(elapsedTime, listEnemyAlive);

		this->CollisionLine(elapsedTime, &pMap->GetListLine());

		this->position += this->velocity*elapsedTime;

		if (this->hp > 1)	//
		{
			if (this->position.x < this->patrolRect.left)
			{
				this->position.x = this->patrolRect.left;
				this->scale.x = this->direction = 1;
			}
			else if (this->position.x > this->patrolRect.right)
			{
				this->position.x = this->patrolRect.right;
				this->scale.x = this->direction = -1;
			}
		}
	}
	break;

	case DIEFLAG_UNKNOWN:
	{
		this->position += this->velocity*elapsedTime;
	}
	break;
	}
}

void CEnemy1::Update_SourceRect(float elapsedTime)
{
	this->elapsedTime += elapsedTime;

	if (this->hp > 1)
		this->sourceRect.left = (int)(this->elapsedTime / ENEMY1_DELAYTIME1) % 2 *
		ENEMY1_WSRCRECT;
	else if (this->hp == 1)
	{
		this->velocity.x != 0 ?
			this->sourceRect.left = (int)(this->elapsedTime / ENEMY1_DELAYTIME0) % 4 *
			ENEMY1_WSRCRECT + ENEMY1_WSRCRECT * 2 :
			this->sourceRect.left = ENEMY1_WSRCRECT * 2;
	}

	this->sourceRect.right = this->sourceRect.left + ENEMY1_WSRCRECT;
}

void CEnemy1::CollisionLine(float elapsedTime,
	std::vector<std::pair<D3DXVECTOR2, D3DXVECTOR2>>* const listLine)
{
	float normalX, normalY;

	for (int i = 0; i < listLine->size(); i++)
	{
		D3DXVECTOR2 point1 = listLine->at(i).first;
		D3DXVECTOR2 point2 = listLine->at(i).second;

		float collisionTime = SweptAABB(
			elapsedTime, this->GetBox(),
			BoudingBox(point1, point2),
			normalX, normalY);

		if (collisionTime < elapsedTime)
		{
			point1.x == point2.x ?
				this->CollisionLineX(
					elapsedTime, collisionTime,
					point1, point2, normalX, normalY) :
				this->CollisionLineY(
					elapsedTime, collisionTime,
					point1, normalX, normalY);
		}
	}
}

void CEnemy1::CollisionLineX(float elapsedTime, float collisionTime,
	D3DXVECTOR2 point1, D3DXVECTOR2 point2, float normalX, float normalY)
{
	float height = std::abs(point1.y - point2.y);

	if (this->velocity.y*elapsedTime < height &&
		this->position.y < point1.y)
	{
		this->velocity.x = (this->velocity.x*collisionTime) / elapsedTime;
		this->scale.x = this->direction = normalX;
	}
}

void CEnemy1::CollisionLineY(float elapsedTime, float collisionTime,
	D3DXVECTOR2 point1, float normalX, float normalY)
{
	if (this->velocity.y < 0.0f &&
		this->position.y >= point1.y)
		this->velocity.y = (point1.y - this->position.y) / elapsedTime;
}

void CEnemy1::CollisionEnemy(float elapsedTime,
	std::map<int, CEnemy*> listEnemyAlive)
{
	float normalX, normalY;

	for (auto& enemy : listEnemyAlive)
	{
		float collisionTime = SweptAABB(
			elapsedTime, this->GetBox(),
			enemy.second->GetBox(),
			normalX, normalY);

		if (collisionTime < elapsedTime &&
			this->id != enemy.second->GetID())
		{
			if (this->hp > 1)		// xả giao
			{
				this->velocity.x = (this->velocity.x*collisionTime) / elapsedTime;
				this->scale.x = this->direction = -this->direction;
			}
			else if (this->IsViolent())	// xả dao
			{
				if (enemy.second->IsViolent())
					this->Die(enemy.second->GetVelocity().x);

				enemy.second->Die(this->velocity.x);
			}
		}
	}
}

void CEnemy1::Render(ID3DXSprite* pSprite, CCamera* const pCamera)
{
	if (!this->IsEnable())
	{
		MATRIX matTransform;

		D3DXVECTOR2 anchorPoint = D3DXVECTOR2(this->GetWSrcRect() / 2.0f,
			this->GetHSrcRect() / 2.0f);

		D3DXVECTOR2 position_temp = pCamera->getPointTransform(
			this->position.x, this->position.y);

		D3DXMatrixTransformation2D(
			&matTransform,
			&anchorPoint, 0, &this->scale,
			&anchorPoint, 0,
			&D3DXVECTOR2(position_temp.x - anchorPoint.x,
				position_temp.y - this->GetHSrcRect()));
		pSprite->SetTransform(&matTransform);

		HR(pSprite->Draw(
			CEnemy1::texture, &this->sourceRect,
			0, 0, D3DCOLOR_XRGB(255, 255, 255)));
	}
}