#pragma once

enum LuigiType
{
	Small = 0,
	Big = 64,
	Fire = 136
};

enum LuigiState
{
	Normal,
	GrowToBig,
	GrowToFire,
	Shink,
	Die
};

enum LuigiMoveType {
	Standing,
	Running,
	Jumping
};

#define LUIGI_WIDTHSIZE 20.0f
#define LUIGI_WIDTHSIZE0 32.0f
#define LUIGI_HEIGHTSIZE 32.0f
#define BIGLUIGI_HEIGHTSIZE 64.0f

#define LUIGI_DIRECTION_LEFT -1
#define LUIGI_DIRECTION_RIGHT 1

#define LUIGI_IIDELAYTIME 0.05f
#define LUIGI_DELAYTIME0 50.0f
#define LUIGI_DELAYTIME1 100.0f

#define LUIGI_WSRCRECT_SMALL 48.0f
#define LUIGI_HSRCRECT_SMALL 64.0f

#define LUIGI_WSRCRECT_BIG 56.0f
#define LUIGI_HSRCRECT_BIG 72.0f

#define LUIGI_GHOST_DELAYTIME 50.0f
#define LUIGI_GHOSTTIME 1000.0f
#define LUIGI_INVINCIBLETIME 60000.0f;
#define LUIGI_FDELAYTIME 200.0f
#define LUIGI_SDELAYTIME 1000.0f

#define LUIGI_KEYUP DIK_UP
#define LUIGI_KEYDOWN DIK_DOWN
#define LUIGI_KEYLEFT DIK_LEFT
#define LUIGI_KEYRIGHT DIK_RIGHT
#define LUIGI_KEYFIRE DIK_A
#define LUIGI_KEYJUMPA DIK_S
#define LUIGI_KEYJUMPB DIK_D

#define LUIGI_IACCELERATIONX 0.00032f
#define LUIGI_DACCELERATIONX 0.00032f
#define LUIGI_LIMITVELOCITYX0 0.32f			//t=1000ms | s=5x32=160px
#define LUIGI_LIMITVELOCITYX1 0.416f		//t=1000ms | s=8x32=256px

#define LUIGI_LIMITVELOCITYY -0.64f

#define LUIGI_DACCELERATIONY 0.0016f
#define LUIGI_JUMPVELOCITYY0 0.56f			//t=300ms | s=3x32=96px
#define LUIGI_JUMPVELOCITYY1 0.64f			//t=400ms | s=4x32=128px
#define LUIGI_JUMPVELOCITYY2 0.72f			//t=500ms | s=5x32=160px

enum RunningFlag {
	RunningFlag0,		//lết bộ
	RunningFlag2,		//lết lẹ
	RunningFlag1,		//cầu tụt
	RunningFlag3,		//lết từ từ
	RunningFlag4,		//phanh
	RunningFlag5		//ngồi lết
};

enum JumpingFlag {
	JumpingFlag0,		//nhúng
	JumpingFlag1,		//nhúng xa
	JumpingFlag2,		//nhúng kiểu
	JumpingFlag3		//nhúng cóc
};

static const int Luigi_Normal_Frame_Standing[3] = { 0,1,2 };
static const int Luigi_Normal_Frame_Running[7] = { 0,3,4,5,6,7,1 };
static const int Luigi_Normal_Frame_Jumping[8] = { 8,9,10,0,11,0,12,1 };

static const int BigLuigi_Normal_Frame_Standing[4] = { 0,1,2,21 };
static const int BigLuigi_Normal_Frame_Running[10] = { 0,3,4,5,6,7,8,9,1,21 };
static const int BigLuigi_Normal_Frame_Jumping[8] = { 10,11,12,0,13,0,14,1 };

static const int Luigi_GrowToBig_Frame[3] = { 22,21,0 };
static const int Luigi_ShrinkToSmall_Frame[3] = { 0,21,22 };
static const int Luigi_Die_Frame[2] = { 18,19 };