#include "CEnemy.h"

void CEnemy::DescreaseHP()
{
	if (this->hp > 0)
		this->hp--;
	else if (this->hp < 0)
		this->hp = 0;

	if (this->hp == 0)
		this->Die1();
}

void CEnemy::Die1()
{
	this->isDied = 1;
}

void CEnemy::Die2()
{
	this->isDied = 2;
}

void CEnemy::Die3(float sign)
{
	sign > 0 ? this->direction = 1 : this->direction = -1;

	this->velocity.x = this->direction*0.16f;
	this->velocity.y = 0.48f;

	this->isDied = 3;
}