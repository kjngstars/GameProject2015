#pragma once

#include <d3dx9.h>

struct BoudingBox
{
	float _x, _y;
	float _w, _h;
	float _vx, _vy;

	BoudingBox() {}

	BoudingBox(float x, float y, float width, float height, float vx, float vy) : _x(x), _y(y), _w(width), _h(height), _vx(vx), _vy(vy)
	{
	}
	BoudingBox(float x, float y, float width, float height) : _x(x), _y(y), _w(width), _h(height), _vx(0), _vy(0)
	{
	}

	BoudingBox(D3DXVECTOR2 point1, D3DXVECTOR2 point2)
	{
		if (point1.x < point2.x)
		{
			this->_x = point1.x;
			this->_w = point2.x - point1.x;
		}
		else
		{
			this->_x = point2.x;
			this->_w = point1.x - point2.x;
		}

		if (point1.y > point2.y)
		{
			this->_y = point1.y;
			this->_h = point1.y - point2.y;
		}
		else
		{
			this->_y = point2.y;
			this->_h = point2.y - point1.y;
		}

		this->_vx = this->_vy = 0.0f;
	}

	bool Intersect(BoudingBox b2)
	{
		bool a4 = this->_y< b2._y - b2._h;
		bool a1 = this->_x + this->_w < b2._x;
		bool a2 = this->_x > b2._x + b2._w;
		bool a3 = this->_y - this->_h > b2._y;

		return !(a1 || a2 || a3 || a4);
	}
};