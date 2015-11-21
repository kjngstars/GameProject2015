#pragma once

struct BoudingBox
{
	float _x, _y;
	float _w, _h;
	float _vx, _vy;

	BoudingBox() {}

	BoudingBox(float x, float y, float width, float height, float vx, float vy) : _x(x),_y(y),_w(width),_h(height),_vx(vx),_vy(vy)
	{
	}
	BoudingBox(float x, float y, float width, float height) : _x(x),_y(y),_w(width),_h(height),_vx(0),_vy(0)
	{
	}

	bool Intersect(BoudingBox b2)		
	{
		bool a4 = this->_y< b2._y - b2._h;
		bool a1 = this->_x + this->_w < b2._x;
		bool a2 = this->_x>b2._x + b2._w;
		bool a3 = this->_y - this->_h > b2._y;
				
		return !(a1 || a2 || a3 || a4);
	}
};