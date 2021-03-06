#include "CEnemy0.h"
#include "../GameConfig.h"
#include "../Collision/Collision.h"

#define ENEMY0_WIDTH 40.0f
#define ENEMY0_HEIGHT0 32.0f
#define ENEMY0_HEIGHT1 64.0f

#define ENEMY0_WSRCRECT 44.0f
#define ENEMY0_HSRCRECT 68.0f

#define ENEMY0_LIMITVELOCITYX 0.08f

#define ENEMY0_DACCELERATIONY 0.0016f
#define ENEMY0_LIMITVELOCITYY -0.64f

#define ENEMY0_DELAYTIME 100.0f

#define ENEMY0_MAXHP 2
#define ENEMY0_TTLDIE 15

// khởi tạo giá trị mặc định CEnemy0::texture là NULL
IDirect3DTexture9* CEnemy0::texture = 0;

// tạo texture dùng chung cho tất cả đối tượng enemy0
void CEnemy0::InitializeTexture(IDirect3DDevice9* pD3DDevice)
{
	if (CEnemy0::texture != nullptr)
		CEnemy0::texture->Release();

	D3DXIMAGE_INFO info;
	HR(D3DXGetImageInfoFromFile("./Content/Enemies/Enemy0.png", &info));
	HR(D3DXCreateTextureFromFileEx(
		pD3DDevice, "./Content/Enemies/Enemy0.png",
		info.Width, info.Height, D3DX_DEFAULT, 0,
		D3DFMT_UNKNOWN, D3DPOOL_MANAGED,
		D3DX_DEFAULT, D3DX_DEFAULT,
		0, &info, 0, &CEnemy0::texture));
}

// hủy texture enemy0
void CEnemy0::ReleaseTexture()
{
	SAFE_RELEASE(CEnemy0::texture)
}

CEnemy0::CEnemy0(EnemyInfo info)
{
	this->id = info._id;
	this->type = info._type;

	this->position.x = info._x + ENEMY0_WSRCRECT / 2.0f;	//chuyển tọa độ x về giữa đối tượng
	this->position.y = info._y;

	if (info._direction == 1)
	{
		this->scale.x = this->direction = -1;	//bên trái
		this->patrolRect.left = info._x - info._patrolWidth;
	}
	else
	{
		this->scale.x = this->direction = 1;	//bên phải
		this->patrolRect.left = info._x;
	}
	this->patrolRect.top = info._y;
	this->patrolRect.right = this->patrolRect.left + info._patrolWidth;
	this->patrolRect.bottom = this->patrolRect.top + info._patrolHeight;

	this->velocity.x = this->direction*ENEMY0_LIMITVELOCITYX;
	this->velocity.y = ENEMY0_LIMITVELOCITYY;

	this->sourceRect = { 0,0,ENEMY0_WSRCRECT ,ENEMY0_HSRCRECT };

	this->hp = ENEMY0_MAXHP;
	this->ttlDie = ENEMY0_TTLDIE;
}

BoudingBox CEnemy0::GetBox()
{
	if (this->hp > 1)
		return BoudingBox(
			this->position.x - ENEMY0_WIDTH / 2.0f,
			this->position.y + ENEMY0_HEIGHT1,
			ENEMY0_WIDTH, ENEMY0_HEIGHT1,
			this->velocity.x,
			this->velocity.y);
	else
		return BoudingBox(
			this->position.x - ENEMY0_WIDTH / 2.0f,
			this->position.y + ENEMY0_HEIGHT0,
			ENEMY0_WIDTH, ENEMY0_HEIGHT0,
			this->velocity.x,
			this->velocity.y);
}

void CEnemy0::Update(float elapsedTime, CMap* const pMap,
	CCamera* const pCamera, std::map<int, CEnemy*> listEnemyAlive)
{
	if (this->IsEnable())
	{
		this->Update_Velocity(elapsedTime);
		this->Update_Collision(elapsedTime, pMap, pCamera, listEnemyAlive);
		this->Update_SourceRect(elapsedTime);
		this->Update_Color(elapsedTime);
	}
}

void CEnemy0::Update_Velocity(float elapsedTime)
{
	if (this->dieFlag != DIEFLAG_BULLET)
	{// enemy không bị bắn chết
		if (this->dieFlag == DIEFLAG_NONE)
		{// chưa ngủm
			if (this->velocity.x*this->direction != ENEMY0_LIMITVELOCITYX)
				this->velocity.x = this->direction*ENEMY0_LIMITVELOCITYX;
		}

		if (this->velocity.y > ENEMY0_LIMITVELOCITYY)
		{// trọng lực
			this->velocity.y -= ENEMY0_DACCELERATIONY*elapsedTime;

			if (this->velocity.y < ENEMY0_LIMITVELOCITYY)
				this->velocity.y = ENEMY0_LIMITVELOCITYY;
		}
	}
}

void CEnemy0::Update_Collision(float elapsedTime, CMap* const pMap,
	CCamera* const pCamera, std::map<int, CEnemy*> listEnemyAlive)
{
	switch (this->dieFlag)
	{
	case DIEFLAG_NONE:// enemy chưa ngủm
	{
		this->CollisionEnemy(elapsedTime, listEnemyAlive);		 // xử lý va chạm với enemy khác

		this->CollisionLine(elapsedTime, &pMap->GetListLine());	// xử lý va chạm với địa hình

		this->position += this->velocity*elapsedTime;

		// kiểm tra đối tượng còn nằm trong vùng di chuyển không
		if (this->position.x < this->patrolRect.left)		// lố bên trái
		{
			this->position.x = this->patrolRect.left;
			this->scale.x = this->direction = 1;
		}
		else if (this->position.x > this->patrolRect.right)	// lố bên phải
		{
			this->position.x = this->patrolRect.right;
			this->scale.x = this->direction = -1;
		}

	}
	break;

	case DIEFLAG_TRAMPLE:// enemy bị đạp chết
	{
		if (this->ttlDie == 0)
			this->dieFlag = -1;
	}
	break;

	case DIEFLAG_BULLET:// enemy bị bắn chết
	{
		this->dieFlag = DIEFLAG_GOODBYE;
	}
	break;

	case DIEFLAG_UNKNOWN:// enemy bị ? mà ngủm
	{
		this->position += this->velocity*elapsedTime;

		BoudingBox viewport(
			pCamera->_position.x, pCamera->_position.y,
			SCREEN_WIDTH, SCREEN_HEIGHT);

		if (!viewport.Intersect(this->GetBox())) // nếu ko nằm trong viewport thì ra đi
			this->dieFlag = DIEFLAG_GOODBYE;
	}
	break;
	}
}

void CEnemy0::Update_SourceRect(float elapsedTime)
{
	if (this->dieFlag == DIEFLAG_NONE)
	{// enemy chưa ngủm
		if (this->hp > 1)
			this->sourceRect.left = (int)(this->elapsedTime / ENEMY0_DELAYTIME) % 2 *
			ENEMY0_WSRCRECT;
		else if (this->hp > 0)
			this->sourceRect.left = (int)(this->elapsedTime / ENEMY0_DELAYTIME) % 2 *
			ENEMY0_WSRCRECT + ENEMY0_WSRCRECT * 2;

		this->sourceRect.right = this->sourceRect.left + ENEMY0_WSRCRECT;
	}
	else if (this->dieFlag == DIEFLAG_TRAMPLE)	// enemy đã hy sinh
	{// bị đạp đầu chết
		if (this->sourceRect.left != ENEMY0_WSRCRECT * 4)
		{
			this->sourceRect.left = ENEMY0_WSRCRECT * 4;
			this->sourceRect.right = this->sourceRect.left + ENEMY0_WSRCRECT;
		}
	}
}

void CEnemy0::Update_Color(float elapsedTime)
{
	this->elapsedTime += elapsedTime;

	if (this->dieFlag == DIEFLAG_TRAMPLE)
	{// enemy bị đạp chết
		if (this->elapsedTime > ENEMY0_DELAYTIME)
		{
			this->color.a == 0.0f ?
				this->color.a = 1.0f : this->color.a = 0.0f;

			this->ttlDie--;

			this->elapsedTime = 0.0f;
		}
	}
}

void CEnemy0::CollisionLine(float elapsedTime,
	std::vector<std::pair<D3DXVECTOR2, D3DXVECTOR2>>* const listLine)
{
	float normalX, normalY;

	for (int i = 0;i < listLine->size();i++)
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

void CEnemy0::CollisionLineX(float elapsedTime, float collisionTime,
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

void CEnemy0::CollisionLineY(float elapsedTime, float collisionTime,
	D3DXVECTOR2 point1, float normalX, float normalY)
{
	if (this->velocity.y < 0.0f &&
		this->position.y >= point1.y)
		this->velocity.y = (point1.y - this->position.y) / elapsedTime;
}

void CEnemy0::CollisionEnemy(float elapsedTime,
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
			this->velocity.x = (this->velocity.x*collisionTime) / elapsedTime;
			this->scale.x = this->direction = -this->direction;
		}
	}
}

void CEnemy0::Render(ID3DXSprite* pSprite, CCamera* const pCamera)
{
	if (this->IsEnable())
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
			CEnemy0::texture, &this->sourceRect,
			0, 0, this->color));
	}
}