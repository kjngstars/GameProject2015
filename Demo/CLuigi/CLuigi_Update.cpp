#include "CLuigi.h"
#include "LuigiConstant.h"

static RunningFlag runningFlag = RunningFlag::RunningFlag0;
static JumpingFlag jumpingFlag = JumpingFlag::JumpingFlag0;

int GetSignNum(float x)
{
	if (x < 0)
		return -1;
	else
		return 1;
}

void CLuigi::Update_Normal(float elapsedTime, CDXInput* inputDevice)
{
	this->Update_Normal_MoveType(inputDevice);
	this->Update_Normal_GhostTime(elapsedTime);
	this->Update_Normal_InvincibleTime(elapsedTime);
	this->Update_Normal_Velocity(elapsedTime, inputDevice);
	this->Update_Normal_DelayTime(elapsedTime, inputDevice);
	this->Update_Normal_Frame(elapsedTime, inputDevice);
	this->Update_Normal_SourceRect();
	this->Update_Normal_Scale();

	this->_position += this->velocity*elapsedTime;

	if (this->_position.y>600.0f)
	{
		this->_position.y = 600.0f;
		this->velocity.y = 0;
		this->moveType = LuigiMoveType::Running;
		this->elapsedTime = this->delayTime;
		this->_scale.x = this->direction*std::abs(this->_scale.x);
	}

	if (inputDevice->KeyPress(DIK_1))
		this->GrowUpToBig();
	else if (inputDevice->KeyPress(DIK_2))
		this->GrowUpToFire();
	else if (inputDevice->KeyPress(DIK_3))
		this->ShrinkToSmall();
	else if (inputDevice->KeyPress(DIK_4))
		this->GoToHeaven();
	else if (inputDevice->KeyPress(DIK_5))
		this->test();
}

void CLuigi::Update_Normal_GhostTime(float elapsedTime)
{
	if (this->ghostTime > 0.0f)
	{
		this->ghostTime -= elapsedTime;
		(int)(this->ghostTime / LUIGI_GHOST_DELAYTIME) % 2 == 1 ?
			this->color.a = 0 : this->color.a = 255;
	}

	if (this->ghostTime < 0.0f)
	{
		this->ghostTime = 0.0f;
		this->color.a = 255;
	}
}

void CLuigi::Update_Normal_InvincibleTime(float elapsedTime)
{
	if (this->invincibleTime > 0.0f)
	{
		this->invincibleTime -= elapsedTime;

		this->hue += 20*this->invincibleTime / LUIGI_INVINCIBLETIME;
		if (this->hue > 360.0f)
			this->hue = 0.0f;

		if (this->invincibleTime < 0.0f)
		{
			this->invincibleTime = 0.0f;
		}
	}
}

void CLuigi::Update_Normal_MoveType(CDXInput* inputDevice)
{
	switch (this->moveType)
	{
	case LuigiMoveType::Standing:
	{
		if (inputDevice->KeyPress(LUIGI_KEYFIRE))
		{
			if (this->fDelayTime == 0.0f &&
				this->type == LuigiType::Fire)
				this->fDelayTime = LUIGI_FDELAYTIME;
		}
		else if (inputDevice->KeyPress(LUIGI_KEYJUMPB))
		{
			this->moveType = LuigiMoveType::Jumping;
			jumpingFlag = JumpingFlag::JumpingFlag2;

			if (this->direction == LUIGI_DIRECTION_LEFT)
				this->index = 3;
			else
				this->index = 5;

			this->velocity.y = LUIGI_JUMPVELOCITYY0;
		}
		else if (inputDevice->KeyPress(LUIGI_KEYJUMPA))
		{
			this->moveType = LuigiMoveType::Jumping;

			inputDevice->KeyDown(LUIGI_KEYDOWN) ?
				jumpingFlag = JumpingFlag::JumpingFlag3 :
				jumpingFlag = JumpingFlag::JumpingFlag0;

			this->velocity.y = LUIGI_JUMPVELOCITYY1;
		}
		else if (!inputDevice->KeyDown(LUIGI_KEYDOWN) &&
			(inputDevice->KeyDown(LUIGI_KEYLEFT) ||
				inputDevice->KeyDown(LUIGI_KEYRIGHT)))
		{
			this->moveType = LuigiMoveType::Running;

			inputDevice->KeyDown(LUIGI_KEYFIRE) ?
				runningFlag = RunningFlag::RunningFlag1 :
				runningFlag = RunningFlag::RunningFlag0;
		}
	}
	break;

	case LuigiMoveType::Running:
	{
		if (inputDevice->KeyPress(LUIGI_KEYFIRE))
		{
			if (this->fDelayTime == 0.0f &&
				this->type == LuigiType::Fire)
				this->fDelayTime = LUIGI_FDELAYTIME;
			else
			{
				if (this->fDelayTime > 0.0f)
					this->fDelayTime = 0.0f;
			}
		}
		else if (inputDevice->KeyPress(LUIGI_KEYJUMPB))
		{
			this->moveType = LuigiMoveType::Jumping;
			jumpingFlag = JumpingFlag::JumpingFlag2;

			if (this->direction == LUIGI_DIRECTION_LEFT)
				this->index = 3;
			else
				this->index = 5;

			std::abs(this->velocity.x) >= LUIGI_LIMITVELOCITYX0 ?
				this->velocity.y = LUIGI_JUMPVELOCITYY1 :
				this->velocity.y = LUIGI_JUMPVELOCITYY0;
		}
		else if (inputDevice->KeyPress(LUIGI_KEYJUMPA))
		{
			this->moveType = LuigiMoveType::Jumping;

			if (std::abs(this->velocity.x) >= LUIGI_LIMITVELOCITYX0)
			{
				this->velocity.y = LUIGI_JUMPVELOCITYY2;

				this->delayTime <= LUIGI_DELAYTIME0 ?
					jumpingFlag = JumpingFlag::JumpingFlag1 :
					jumpingFlag = JumpingFlag::JumpingFlag0;
			}
			else
			{
				this->velocity.y = LUIGI_JUMPVELOCITYY1;
				jumpingFlag = JumpingFlag::JumpingFlag0;
			}

			if (inputDevice->KeyDown(LUIGI_KEYDOWN))
				jumpingFlag = JumpingFlag::JumpingFlag3;
		}
		else if (this->velocity == D3DXVECTOR2(0.0f, 0.0f))
			this->moveType = LuigiMoveType::Standing;
	}
	break;

	case LuigiMoveType::Jumping:
	{
		if (inputDevice->KeyPress(LUIGI_KEYFIRE) &&
			jumpingFlag != JumpingFlag::JumpingFlag2)
		{
			if (this->fDelayTime == 0.0f &&
				this->type == LuigiType::Fire)
				this->fDelayTime = LUIGI_FDELAYTIME;
		}
	}
	break;
	}
}

void CLuigi::Update_Normal_Velocity(float elapsedTime, CDXInput* inputDevice)
{
	switch (this->moveType)
	{
	case LuigiMoveType::Standing:
	{

	}
	break;

	case LuigiMoveType::Running:
	{
		if (inputDevice->KeyDown(LUIGI_KEYDOWN))
			runningFlag = RunningFlag::RunningFlag5;
		else if (inputDevice->KeyDown(LUIGI_KEYLEFT))
		{
			this->direction = LUIGI_DIRECTION_LEFT;

			inputDevice->KeyDown(LUIGI_KEYFIRE) ?
				runningFlag = RunningFlag::RunningFlag1 :
				runningFlag = RunningFlag::RunningFlag0;
		}
		else if (inputDevice->KeyDown(LUIGI_KEYRIGHT))
		{
			this->direction = LUIGI_DIRECTION_RIGHT;

			inputDevice->KeyDown(LUIGI_KEYFIRE) ?
				runningFlag = RunningFlag::RunningFlag1 :
				runningFlag = RunningFlag::RunningFlag0;
		}
		else
			runningFlag = RunningFlag::RunningFlag3;

		if (this->direction*this->velocity.x < 0)
			runningFlag = RunningFlag::RunningFlag4;

		switch (runningFlag)
		{
		case RunningFlag::RunningFlag0:
		{
			this->IncreaseVelocityX(elapsedTime, LUIGI_LIMITVELOCITYX0);
		}
		break;

		case RunningFlag::RunningFlag1:
		{
			this->IncreaseVelocityX(elapsedTime, LUIGI_LIMITVELOCITYX1);
		}
		break;

		case RunningFlag::RunningFlag2:
		{

		}
		break;

		case RunningFlag::RunningFlag3:
		{
			this->DecreaseVelocityX(elapsedTime, 1.0f);
		}
		break;

		case RunningFlag::RunningFlag4:
		{
			this->DecreaseVelocityX(elapsedTime, 2.0f);
		}
		break;

		case RunningFlag::RunningFlag5:
		{
			this->DecreaseVelocityX(elapsedTime, 2.0f);
		}
		break;
		}
	}
	break;

	case LuigiMoveType::Jumping:
	{
		if (inputDevice->KeyDown(LUIGI_KEYLEFT) ||
			inputDevice->KeyDown(LUIGI_KEYRIGHT))
		{
			if (inputDevice->KeyDown(LUIGI_KEYLEFT))
				this->direction = LUIGI_DIRECTION_LEFT;
			else if (inputDevice->KeyDown(LUIGI_KEYRIGHT))
				this->direction = LUIGI_DIRECTION_RIGHT;

			if (this->direction*this->velocity.x < 0)
				this->DecreaseVelocityX(elapsedTime, 2.0f);
			else
			{
				inputDevice->KeyDown(LUIGI_KEYFIRE) ?
					this->IncreaseVelocityX(elapsedTime, LUIGI_LIMITVELOCITYX1) :
					this->IncreaseVelocityX(elapsedTime, LUIGI_LIMITVELOCITYX0);
			}
		}

		switch (jumpingFlag)
		{
		case JumpingFlag::JumpingFlag0:
		{
			inputDevice->KeyDown(LUIGI_KEYJUMPA) ?
				this->IncreaseVelocityY(elapsedTime, 1.0f) :
				this->IncreaseVelocityY(elapsedTime, 2.0f);
		}
		break;

		case JumpingFlag::JumpingFlag1:
		{
			inputDevice->KeyDown(LUIGI_KEYJUMPA) ?
				this->IncreaseVelocityY(elapsedTime, 1.0f) :
				this->IncreaseVelocityY(elapsedTime, 2.0f);
		}
		break;

		case JumpingFlag::JumpingFlag2:
		{
			inputDevice->KeyDown(LUIGI_KEYJUMPB) ?
				this->IncreaseVelocityY(elapsedTime, 1.0f) :
				this->IncreaseVelocityY(elapsedTime, 2.0f);
		}
		break;

		case JumpingFlag::JumpingFlag3:
		{
			inputDevice->KeyDown(LUIGI_KEYJUMPA) ?
				this->IncreaseVelocityY(elapsedTime, 1.0f) :
				this->IncreaseVelocityY(elapsedTime, 2.0f);
		}
		break;
		}
	}
	break;
	}
}

void CLuigi::Update_Normal_DelayTime(float elapsedTime, CDXInput* inputDevice)
{
	switch (this->moveType)
	{
	case LuigiMoveType::Standing:
	{
		if (this->delayTime < LUIGI_DELAYTIME1)
		{
			this->delayTime += LUIGI_IIDELAYTIME*elapsedTime;

			if (this->delayTime > LUIGI_DELAYTIME1)
				this->delayTime = LUIGI_DELAYTIME1;
		}
	}
	break;

	case LuigiMoveType::Running:
	{
		if (runningFlag == RunningFlag::RunningFlag1)
		{
			if (this->delayTime > LUIGI_DELAYTIME0)
			{
				this->delayTime -= LUIGI_IIDELAYTIME*elapsedTime;

				if (this->delayTime < LUIGI_DELAYTIME0)
					this->delayTime = LUIGI_DELAYTIME0;
			}
		}
		else
		{
			if (this->delayTime < LUIGI_DELAYTIME1)
			{
				this->delayTime += LUIGI_IIDELAYTIME*elapsedTime;

				if (this->delayTime > LUIGI_DELAYTIME1)
					this->delayTime = LUIGI_DELAYTIME1;
			}
		}
	}
	break;

	case LuigiMoveType::Jumping:
	{
		if ((inputDevice->KeyDown(LUIGI_KEYLEFT) || inputDevice->KeyDown(LUIGI_KEYRIGHT)) &&
			inputDevice->KeyDown(LUIGI_KEYFIRE))
		{
			if (this->delayTime > LUIGI_DELAYTIME0)
			{
				this->delayTime -= LUIGI_IIDELAYTIME*elapsedTime;

				if (this->delayTime < LUIGI_DELAYTIME0)
					this->delayTime = LUIGI_DELAYTIME0;
			}
		}
		else
		{
			if (this->delayTime < LUIGI_DELAYTIME1)
			{
				this->delayTime += LUIGI_IIDELAYTIME*elapsedTime;

				if (this->delayTime > LUIGI_DELAYTIME1)
					this->delayTime = LUIGI_DELAYTIME1;
			}
		}
	}
	break;
	}
}

void CLuigi::Update_Normal_Frame(float elapsedTime, CDXInput* inputDevice)
{
	this->elapsedTime += elapsedTime;

	if (this->type == LuigiType::Small)
	{
		switch (this->moveType)
		{
		case LuigiMoveType::Standing:
		{
			if (inputDevice->KeyDown(LUIGI_KEYDOWN))
				this->index = 1;
			else if (inputDevice->KeyDown(LUIGI_KEYUP))
				this->index = 2;
			else if (this->frame != Luigi_Normal_Frame_Standing[0])
				this->index = 0;

			this->frame = Luigi_Normal_Frame_Standing[this->index];
		}
		break;

		case LuigiMoveType::Running:
		{
			if (this->elapsedTime > this->delayTime)
			{
				switch (runningFlag)
				{
				case RunningFlag::RunningFlag0:
				{
					this->index++;
					if (this->index > 1) this->index = 0;
				}
				break;

				case RunningFlag::RunningFlag1:
				{
					this->index++;

					if (std::abs(this->velocity.x) == LUIGI_LIMITVELOCITYX1)
					{
						if (this->index > 3) this->index = 2;
					}
					else
						if (this->index > 1) this->index = 0;
				}
				break;

				case RunningFlag::RunningFlag2:
				{
					this->index = 4;
				}
				break;

				case RunningFlag::RunningFlag3:
				{
					this->index++;
					if (this->index > 1) this->index = 0;
				}
				break;

				case RunningFlag::RunningFlag4:
				{
					this->index = 5;
				}
				break;

				case RunningFlag::RunningFlag5:
				{
					this->index = 6;
				}
				break;
				}
			}

			this->frame = Luigi_Normal_Frame_Running[this->index];
		}
		break;

		case LuigiMoveType::Jumping:
		{
			switch (jumpingFlag)
			{
			case JumpingFlag::JumpingFlag0:
			{
				this->velocity.y < 0.0f ?
					this->index = 0 : this->index = 1;
			}
			break;

			case JumpingFlag::JumpingFlag1:
			{
				this->index = 2;
			}
			break;

			case JumpingFlag::JumpingFlag2:
			{
				if (this->elapsedTime > 0.5f*LUIGI_DELAYTIME0)
				{
					this->index++;

					if (this->index > 6)
						this->index = 3;

					this->elapsedTime = 0.0f;
				}
			}
			break;

			case JumpingFlag::JumpingFlag3:
			{
				this->index = 7;
			}
			break;
			}

			this->frame = Luigi_Normal_Frame_Jumping[this->index];
		}
		break;
		}
	}
	else
	{
		switch (this->moveType)
		{
		case LuigiMoveType::Standing:
		{
			if (this->fDelayTime > 0.0f)
			{
				this->index = 3;

				this->fDelayTime -= elapsedTime;
				if (this->fDelayTime < 0.0f)
					this->fDelayTime = 0.0f;
			}
			else if (inputDevice->KeyDown(LUIGI_KEYDOWN))
				this->index = 1;
			else if (inputDevice->KeyDown(LUIGI_KEYUP))
				this->index = 2;
			else if (this->frame != BigLuigi_Normal_Frame_Standing[0])
				this->index = 0;

			this->frame = BigLuigi_Normal_Frame_Standing[this->index];
		}
		break;

		case LuigiMoveType::Running:
		{
			if (this->fDelayTime > 0.0f)
			{
				this->index = 9;

				this->fDelayTime -= elapsedTime;
				if (this->fDelayTime < 0.0f)
				{
					this->fDelayTime = 0.0f;
					goto RunningLabel0;
				}
			}
			else if (this->elapsedTime > this->delayTime)
			{
			RunningLabel0:
				switch (runningFlag)
				{
				case RunningFlag::RunningFlag0:
				{
					this->index++;
					if (this->index > 2) this->index = 0;
				}
				break;

				case RunningFlag::RunningFlag1:
				{
					this->index++;

					if (std::abs(this->velocity.x) == LUIGI_LIMITVELOCITYX1)
					{
						if (this->index > 5) this->index = 3;
					}
					else
						if (this->index > 2) this->index = 0;
				}
				break;

				case RunningFlag::RunningFlag2:
				{
					this->index = 8;
				}
				break;

				case RunningFlag::RunningFlag3:
				{
					this->index++;
					if (this->index > 2) this->index = 0;
				}
				break;

				case RunningFlag::RunningFlag4:
				{
					this->index = 7;
				}
				break;

				case RunningFlag::RunningFlag5:
				{
					this->index = 8;
				}
				break;
				}
			}

			this->frame = BigLuigi_Normal_Frame_Running[this->index];
		}
		break;

		case LuigiMoveType::Jumping:
		{
			if (this->fDelayTime > 0.0f)
			{
				this->index = 2;

				this->fDelayTime -= elapsedTime;
				if (this->fDelayTime < 0.0f)
				{
					this->fDelayTime = 0.0f;
					this->elapsedTime = this->delayTime;
				}
			}
			else
			{
				switch (jumpingFlag)
				{
				case JumpingFlag::JumpingFlag0:
				{
					this->velocity.y < 0.0f ?
						this->index = 0 : this->index = 1;
				}
				break;

				case JumpingFlag::JumpingFlag1:
				{
					this->index = 2;
				}
				break;

				case JumpingFlag::JumpingFlag2:
				{
					if (this->elapsedTime > 0.5f*LUIGI_DELAYTIME0)
					{
						this->index++;

						if (this->index > 6)
							this->index = 3;

						this->elapsedTime = 0.0f;
					}
				}
				break;

				case JumpingFlag::JumpingFlag3:
				{
					this->index = 7;
				}
				break;
				}
			}

			this->frame = BigLuigi_Normal_Frame_Jumping[this->index];
		}
		break;
		}
	}

	if (this->elapsedTime > this->delayTime)
		this->elapsedTime = 0.0f;
}

void CLuigi::Update_Normal_SourceRect()
{
	if (this->type == LuigiType::Small)
	{
		this->sourceRect.left = this->frame*LUIGI_WSRCRECT_SMALL;
		this->sourceRect.right = this->sourceRect.left + LUIGI_WSRCRECT_SMALL;
		this->sourceRect.top = this->type;
		this->sourceRect.bottom = this->sourceRect.top + LUIGI_HSRCRECT_SMALL;
	}
	else
	{
		this->sourceRect.left = this->frame*LUIGI_WSRCRECT_BIG;
		this->sourceRect.right = this->sourceRect.left + LUIGI_WSRCRECT_BIG;
		this->sourceRect.top = this->type;
		this->sourceRect.bottom = this->sourceRect.top + LUIGI_HSRCRECT_BIG;
	}
}

void CLuigi::Update_Normal_Scale()
{
	if (this->moveType == LuigiMoveType::Jumping &&
		jumpingFlag == JumpingFlag::JumpingFlag2)
	{
		if (this->index == 3)
			this->_scale.x = -std::abs(this->_scale.x);

		if (this->index == 5)
			this->_scale.x = std::abs(this->_scale.x);
	}
	else
	{
		if (this->elapsedTime == 0.0f)
			this->_scale.x = this->direction*std::abs(this->_scale.x);
	}
}

void CLuigi::IncreaseVelocityX(float elapsedTime, float limit)
{
	float temp = std::abs(this->velocity.x);

	if (temp < limit)
	{
		temp = std::abs(this->velocity.x) +
			LUIGI_IACCELERATIONX*elapsedTime;

		if (temp > limit)
			temp = limit;

		this->velocity.x == 0.0f ?
			this->velocity.x = this->direction*temp :
			this->velocity.x = GetSignNum(this->velocity.x)*temp;
	}
	else if (temp > limit)
		this->DecreaseVelocityX(elapsedTime, 1);
}

void CLuigi::DecreaseVelocityX(float elapsedTime, float k)
{
	if (this->velocity.x != 0.0f)
	{
		float temp = std::abs(this->velocity.x) -
			k*LUIGI_DACCELERATIONX*elapsedTime;

		if (temp > 0.0f)
			this->velocity.x = GetSignNum(this->velocity.x)*temp;
		else
			this->velocity.x = 0.0f;
	}
}

void CLuigi::IncreaseVelocityY(float elapsedTime, float k)
{
	if (this->velocity.y < LUIGI_LIMITVELOCITYY)
	{
		this->velocity.y += k*LUIGI_DACCELERATIONY*elapsedTime;

		if (this->velocity.y > LUIGI_LIMITVELOCITYY)
			this->velocity.y = LUIGI_LIMITVELOCITYY;
	}
}

void CLuigi::Update_GrowToBig(float elapsedTime)
{
	if (this->sDelayTime > 0.0f)
	{
		this->elapsedTime += elapsedTime;

		if (this->elapsedTime > (LUIGI_SDELAYTIME / 10.0f))
		{
			if (this->sDelayTime > LUIGI_SDELAYTIME / 2.0f)
			{
				this->index++;
				if (this->index > 1) this->index = 0;
			}
			else
			{
				this->index++;
				if (this->index > 2) this->index = 1;
			}

			this->frame = Luigi_GrowToBig_Frame[this->index];

			this->sourceRect.left = this->frame*LUIGI_WSRCRECT_BIG;
			this->sourceRect.right = this->sourceRect.left + LUIGI_WSRCRECT_BIG;
			this->sourceRect.top = this->type;
			this->sourceRect.bottom = this->sourceRect.top + LUIGI_HSRCRECT_BIG;

			this->elapsedTime = 0.0f;
		}

		this->sDelayTime -= elapsedTime;
		if (this->sDelayTime < 0.0f)
		{
			this->state = LuigiState::Normal;
			this->sDelayTime = 0.0f;
			this->elapsedTime = this->delayTime;
			this->type = LuigiType::Big;
		}
	}
	else
	{
		this->sDelayTime = 0.0f;
		this->elapsedTime = this->delayTime;
		this->type = LuigiType::Big;
	}
}

void CLuigi::Update_GrowToFire(float elapsedTime)
{
	if (this->sDelayTime > 0.0f)
	{
		this->elapsedTime += elapsedTime;

		if (this->elapsedTime > (LUIGI_SDELAYTIME / 10.0f))
		{

			this->type == LuigiType::Big ?
				this->type = LuigiType::Fire :
				this->type = LuigiType::Big;

			this->sourceRect.left = this->frame*LUIGI_WSRCRECT_BIG;
			this->sourceRect.right = this->sourceRect.left + LUIGI_WSRCRECT_BIG;
			this->sourceRect.top = this->type;
			this->sourceRect.bottom = this->sourceRect.top + LUIGI_HSRCRECT_BIG;

			this->elapsedTime = 0.0f;
		}

		this->sDelayTime -= elapsedTime;
		if (this->sDelayTime < 0.0f)
		{
			this->state = LuigiState::Normal;
			this->sDelayTime = 0.0f;
			this->elapsedTime = this->delayTime;
			this->type = LuigiType::Fire;
		}
	}
	else
	{
		this->sDelayTime = 0.0f;
		this->elapsedTime = this->delayTime;
		this->type = LuigiType::Fire;
	}
}

void CLuigi::Update_ShrinkToSmall(float elapsedTime)
{

	if (this->sDelayTime > 0.0f)
	{
		this->elapsedTime += elapsedTime;

		if (this->elapsedTime > (LUIGI_SDELAYTIME / 10.0f))
		{
			if (this->sDelayTime > LUIGI_SDELAYTIME / 2.0f)
			{
				this->index++;
				if (this->index > 1) this->index = 0;
			}
			else
			{
				this->index++;
				if (this->index > 2) this->index = 1;
			}

			this->frame = Luigi_ShrinkToSmall_Frame[this->index];

			this->sourceRect.left = this->frame*LUIGI_WSRCRECT_BIG;
			this->sourceRect.right = this->sourceRect.left + LUIGI_WSRCRECT_BIG;
			this->sourceRect.top = this->type;
			this->sourceRect.bottom = this->sourceRect.top + LUIGI_HSRCRECT_BIG;

			this->elapsedTime = 0.0f;
		}

		this->sDelayTime -= elapsedTime;
		if (this->sDelayTime < 0.0f)
		{
			this->state = LuigiState::Normal;
			this->sDelayTime = 0.0f;
			this->elapsedTime = this->delayTime;
			this->ghostTime = LUIGI_GHOSTTIME;
			this->type = LuigiType::Small;
		}
	}
	else
	{
		this->sDelayTime = 0.0f;
		this->elapsedTime = this->delayTime;
		this->type = LuigiType::Big;
	}
}

void CLuigi::Update_Die(float elapsedTime)
{
	if (this->velocity.y < LUIGI_LIMITVELOCITYY)
	{
		this->velocity.y += LUIGI_DACCELERATIONY*elapsedTime;

		if (this->velocity.y > LUIGI_LIMITVELOCITYY)
			this->velocity.y = LUIGI_LIMITVELOCITYY;
	}

	this->elapsedTime += elapsedTime;
	if (this->elapsedTime > LUIGI_DELAYTIME0)
	{
		this->index++;
		if (this->index > 1) this->index = 0;

		this->frame = Luigi_Die_Frame[this->index];

		this->sourceRect.left = this->frame*LUIGI_WSRCRECT_SMALL;
		this->sourceRect.right = this->sourceRect.left + LUIGI_WSRCRECT_SMALL;
		this->sourceRect.top = this->type;
		this->sourceRect.bottom = this->sourceRect.top + LUIGI_HSRCRECT_SMALL;

		this->elapsedTime = 0.0f;
	}

	this->_position += this->velocity*elapsedTime;
}

void CLuigi::Update(float elapsedTime, CDXInput* inputDevice)
{
	switch (this->state)
	{
	case LuigiState::Normal:
	{
		this->Update_Normal(elapsedTime, inputDevice);
	}
	break;

	case LuigiState::GrowToBig:
	{
		this->Update_GrowToBig(elapsedTime);
	}
	break;

	case LuigiState::GrowToFire:
	{
		this->Update_GrowToFire(elapsedTime);
	}
	break;

	case LuigiState::Shink:
	{
		this->Update_ShrinkToSmall(elapsedTime);
	}
	break;

	case LuigiState::Die:
	{
		this->Update_Die(elapsedTime);
	}
	break;
	}
}