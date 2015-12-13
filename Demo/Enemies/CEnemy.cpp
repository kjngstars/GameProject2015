#include "CEnemiesManager.h"

void CEnemy::DescreaseHP()
{
	if (this->hp > 0)
		this->hp--;
	else if (this->hp < 0)
		this->hp = 0;

	if (this->hp == 0)
		this->TrampledToDeath();
}

void CEnemy::TrampledToDeath()
{
	this->dieFlag = DIEFLAG_TRAMPLE;

	CScoreManager::AddScore(this->score);
}

void CEnemy::BulletHit()
{
	this->dieFlag = DIEFLAG_BULLET;

	CScoreManager::AddScore(this->score);
}

void CEnemy::Die(float sign)
{
	sign > 0 ? this->direction = 1 : this->direction = -1;

	this->velocity.x = this->direction*0.16f;
	this->velocity.y = 0.48f;

	CEnemiesManager::Playsound();
	this->dieFlag = DIEFLAG_UNKNOWN;

	CScoreManager::AddScore(this->score);
}