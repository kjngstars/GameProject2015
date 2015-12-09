#include "CLuigiBullet.h"
#include "../GameConfig.h"
#include "../CLuigi/LuigiConstant.h"
#include "../Collision/Collision.h"
#include "../GameConfig.h"
#include "../Enemies/CEnemiesManager.h"

#define LUIGIBULLET_WIDTH 16.0f
#define LUIGIBULLET_HEIGHT 16.0f

#define LUIGIBULLET_WSRCRECT 64.0f
#define LUIGIBULLET_HSRCRECT 64.0f

#define LUIGIBULLET_DELAYTIME 50.0f
#define LUIGIBULLET_TTL 11.0f

#define LUIGIBULLET_LIMITVELOCITYX 0.512f

#define LUIGIBULLET_DACCELERATIONY 0.0016f
#define LUIGIBULLET_VELOCITYY 0.32f
#define LUIGIBULLET_LIMITVELOCITYY -0.64f

#define LUIGIBULLET_ANGULARACCELERRATION 0.4f

#define LIMIT_LUIGIBULLET 200.0f

static const D3DXVECTOR2 LuigiBulletAnchorPoint =
D3DXVECTOR2(32.0f, 32.0f);

IDirect3DTexture9* CLuigiBulletManager::bulletTexture;
std::vector<CLuigiBullet> CLuigiBulletManager::listBullet;

CLuigiBullet::CLuigiBullet(D3DXVECTOR2 position, float direction)
{
	this->position = { position.x,
		position.y + LUIGI_HEIGHTSIZE };

	this->velocity = { direction*LUIGIBULLET_LIMITVELOCITYX,
		LUIGIBULLET_LIMITVELOCITYY };

	this->rotate = 0.0f;
	this->elapsedTime = 0.0f;
	this->ttl = -1.0f;
	this->sourceRect = { 0,0,LUIGIBULLET_WSRCRECT,LUIGIBULLET_WSRCRECT };
}

BoudingBox CLuigiBullet::GetBox()
{
	return BoudingBox(
		this->position.x - LUIGIBULLET_WIDTH / 2.0f,
		this->position.y + LUIGIBULLET_HEIGHT,
		LUIGIBULLET_WIDTH, LUIGIBULLET_HEIGHT,
		this->velocity.x, this->velocity.y);
}

void CLuigiBullet::Update(float elapsedTime, CMap* const pMap)
{
	if (this->ttl < 0.0f)
	{
		static bool checkVelocityY;

		if (this->velocity.y > LUIGIBULLET_LIMITVELOCITYY)
		{
			this->velocity.y -= LUIGIBULLET_DACCELERATIONY*elapsedTime;

			if (this->velocity.y < LUIGIBULLET_LIMITVELOCITYY)
				this->velocity.y = LUIGIBULLET_LIMITVELOCITYY;
		}
		this->velocity.x > 0.0f ?
			this->rotate -= LUIGIBULLET_ANGULARACCELERRATION :
			this->rotate += LUIGIBULLET_ANGULARACCELERRATION;

#pragma region Object
		std::vector<std::pair<D3DXVECTOR2, D3DXVECTOR2>> listLine =
			pMap->GetListLine();

		float normalX, normalY;

		for (int i = 0;i < listLine.size();i++)
		{
			D3DXVECTOR2 point1 = listLine.at(i).first;
			D3DXVECTOR2 point2 = listLine.at(i).second;

			float collisionTime = SweptAABB(
				elapsedTime, this->GetBox(),
				BoudingBox(point1, point2),
				normalX, normalY);

			if (collisionTime < elapsedTime)
			{
				if (point1.x == point2.x)
				{
					float height = std::abs(point1.y - point2.y);

					if (this->velocity.y*elapsedTime < height &&
						this->position.y < point1.y)
					{
						this->velocity.x = this->velocity.x*collisionTime / elapsedTime;
						this->ttl = LUIGIBULLET_TTL;
					}
				}
				else
				{
					this->velocity.y = (point1.y - this->position.y) / elapsedTime;
					checkVelocityY = true;
				}
			}
		}

#pragma endregion

#pragma region enemies
		for (auto& enemy : CEnemiesManager::GetListEnemyAlive())
		{

			float collisionTime = SweptAABB(
				elapsedTime, this->GetBox(),
				enemy.second->GetBox(),
				normalX, normalY);

			if (collisionTime < elapsedTime)
			{
				checkVelocityY = false;
				enemy.second->Die2();
				this->velocity.x = this->velocity.x*collisionTime / elapsedTime;
				this->ttl = LUIGIBULLET_TTL;
			}
		}
#pragma endregion

		this->position += this->velocity*elapsedTime;

		if (checkVelocityY == true)
		{
			this->velocity.y = LUIGIBULLET_VELOCITYY;
			checkVelocityY = false;
		}
	}
	else if (this->ttl > 0.0f)
	{
		if (this->rotate != 0.0f)
			this->rotate = 0.0f;

		this->elapsedTime += elapsedTime;

		if (this->elapsedTime > LUIGIBULLET_DELAYTIME)
		{
			this->sourceRect.left += LUIGIBULLET_WSRCRECT;
			this->sourceRect.right = this->sourceRect.left + LUIGIBULLET_WSRCRECT;

			this->ttl--;
			this->elapsedTime = 0.0f;
		}
	}
}

void CLuigiBulletManager::Initialize(IDirect3DDevice9* pD3DDevice)
{
	if (CLuigiBulletManager::bulletTexture != nullptr)
		CLuigiBulletManager::bulletTexture->Release();

	D3DXIMAGE_INFO info;
	HR(D3DXGetImageInfoFromFile("./Content/Luigi/LuigiBullet.png", &info));
	HR(D3DXCreateTextureFromFileEx(
		pD3DDevice, "./Content/Luigi/LuigiBullet.png",
		info.Width, info.Height, D3DX_DEFAULT, 0,
		D3DFMT_UNKNOWN, D3DPOOL_MANAGED,
		D3DX_DEFAULT, D3DX_DEFAULT,
		0, &info, 0, &CLuigiBulletManager::bulletTexture));
}

void CLuigiBulletManager::Destroy()
{
	SAFE_RELEASE(CLuigiBulletManager::bulletTexture);
	CLuigiBulletManager::listBullet.clear();
}

void CLuigiBulletManager::Update(float elapsedTime, CCamera* const pCamera, CMap* const pMap)
{
	BoudingBox viewport = BoudingBox(
		pCamera->_position.x, pCamera->_position.y,
		SCREEN_WIDTH, SCREEN_HEIGHT);

	for (int i = 0;i < CLuigiBulletManager::listBullet.size();i++)
	{
		CLuigiBulletManager::listBullet.at(i).Update(elapsedTime, pMap);

		if (CLuigiBulletManager::listBullet.at(i).ttl == 0.0f ||
			!viewport.Intersect(CLuigiBulletManager::listBullet.at(i).GetBox()))
		{
			CLuigiBulletManager::listBullet.erase(
				CLuigiBulletManager::listBullet.begin() + i);
			i--;
		}
	}
}

void CLuigiBulletManager::Render(ID3DXSprite* pSprite, CCamera* const pCamera)
{
	for (int i = 0;i < CLuigiBulletManager::listBullet.size();i++)
	{
		MATRIX matTransform;

		D3DXVECTOR2 position_temp = pCamera->getPointTransform(
			CLuigiBulletManager::listBullet.at(i).position.x,
			CLuigiBulletManager::listBullet.at(i).position.y);

		D3DXMatrixTransformation2D(
			&matTransform,
			&LuigiBulletAnchorPoint, 0,
			&D3DXVECTOR2(1.0f, 1.0f),
			&LuigiBulletAnchorPoint, CLuigiBulletManager::listBullet.at(i).rotate,
			&D3DXVECTOR2(
				position_temp.x - LuigiBulletAnchorPoint.x,
				position_temp.y - LuigiBulletAnchorPoint.y*1.25f));
		pSprite->SetTransform(&matTransform);

		pSprite->Draw(
			CLuigiBulletManager::bulletTexture,
			&CLuigiBulletManager::listBullet.at(i).sourceRect,
			0, 0, D3DCOLOR_XRGB(255, 255, 255));
	}
}

bool CLuigiBulletManager::Shoot(D3DXVECTOR2 position, int direction)
{
	if (CLuigiBulletManager::listBullet.size() < LIMIT_LUIGIBULLET)
	{
		listBullet.push_back(CLuigiBullet(position, direction));
		return true;
	}
	else
		return false;
}