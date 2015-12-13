#include "CEnemiesManager.h"
#include "../Scene/CScoreManager.h"

void CEnemy::DescreaseHP()
{
	if (this->hp > 0)
		this->hp--;
	else if (this->hp < 0)
		this->hp = 0;

	if (this->hp == 0)
		this->dieFlag = DIEFLAG_TRAMPLE;

	CScoreManager::AddScore(
		D3DXVECTOR2(this->GetBox()._x + this->GetBox()._w,
			this->GetBox()._y));
}

void CEnemy::TrampledToDeath()
{
	this->dieFlag = DIEFLAG_TRAMPLE;

	CScoreManager::AddScore(
		D3DXVECTOR2(this->GetBox()._x + this->GetBox()._w,
			this->GetBox()._y));
}

void CEnemy::BulletHit()
{
	this->dieFlag = DIEFLAG_BULLET;

	CScoreManager::AddScore(
		D3DXVECTOR2(this->GetBox()._x + this->GetBox()._w,
			this->GetBox()._y));
}

void CEnemy::Die(float sign)
{
	sign > 0 ? this->direction = 1 : this->direction = -1;

	this->velocity.x = this->direction*0.16f;
	this->velocity.y = 0.48f;

	CEnemiesManager::Playsound();
	this->dieFlag = DIEFLAG_UNKNOWN;

	CScoreManager::AddScore(
		D3DXVECTOR2(this->GetBox()._x + this->GetBox()._w,
			this->GetBox()._y));
}

bool CEnemy::Dame(float sign)
{
	return this->dieFlag == DIEFLAG_NONE;
}