#include "Camera.h"



CCamera::CCamera()
{
	D3DXMatrixIdentity(&_matrixTransform);
	_matrixTransform._22 = -1;
	_position.x = 0;

	_startX = 0;
	_endX = 0;
	_position.y = SCREEN_HEIGHT;

	
}

CCamera::CCamera(int start, int end)
{
	D3DXMatrixIdentity(&_matrixTransform);
	_matrixTransform._22 = -1;
	_position.x = 0;

	_startX = start;
	_endX = end;
	_position.y = SCREEN_HEIGHT;

	_x = 0;
	_y = SCREEN_HEIGHT;
}


CCamera::~CCamera()
{
}

VECTOR2 CCamera::getPointTransform(float x, float y)
{
	D3DXVECTOR3 pos1(x, y, 0);

	D3DXVECTOR4 result;
	D3DXVec3Transform(&result, &pos1, &_matrixTransform);

	return D3DXVECTOR2(result.x, result.y);
}

void CCamera::update(float x, float y)
{
	_position.x = x - SCREEN_WIDTH / 2;

	if (_position.x < _startX)
	{
		_position.x = _startX;
	}
	if (_position.x + SCREEN_WIDTH > _endX) //1024 = length of map 1
	{
		_position.x = _endX - SCREEN_WIDTH;
	}


	_matrixTransform._41 = -_position.x;
	_matrixTransform._42 = _position.y;
}

void CCamera::moveLeft()
{
	_x -= 5;
}

void CCamera::moveRight()
{
	_x+= 5;
}
