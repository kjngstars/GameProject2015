#pragma once

#include <d3d9.h>
#include <d3dx9.h>
#include "GameConfig.h"

class CCamera
{
public:
	CCamera();
	CCamera(int start, int end);
	~CCamera();

	VECTOR2 _position;
	D3DXMATRIX _matrixTransform;
	int _startX, _endX;

	VECTOR2 getPointTransform(float x, float y);
	void update(float x, float y);

	void moveLeft();
	void moveRight();

	int _x, _y;
};

