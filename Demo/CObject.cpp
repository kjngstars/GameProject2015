#include "CObject.h"

CObject::CObject()
{
}

CObject::CObject(std::string fileName)
{
	D3DXIMAGE_INFO info = CREATE_TEXTURE(fileName.c_str(), _texture);
	_width = (float)info.Width;
	_height = (float)info.Height;						  
	//_anchorPoint = ccp(float(_width / 2), float(_height / 2));
	//set anchor point at (bottom,left)
	_anchorPoint = ccp(0, _width);
	_rotate = 0.0f;
	_scale = ccp(1.0f, 1.0f);
	_position.x = 0;
	_position.y = 0;
	_color = D3DCOLOR_XRGB(255, 255, 255);
		
}

CObject::CObject(std::string fileName, int x, int y)
{
	D3DXIMAGE_INFO info = CREATE_TEXTURE(fileName.c_str(), _texture);
	_width = (float)info.Width;
	_height = (float)info.Height;		
	_rotate = 0.0f;
	_scale = ccp(1.0f, 1.0f);
	_position.x = (float)x;
	_position.y = (float)y;
	_color = D3DCOLOR_XRGB(255, 255, 255);
}

CObject::CObject(int x, int y, int width, int height) : _position(x, y), _width(width),_height(height)
{
}

CObject::CObject(std::string fileName, D3DCOLOR color)
{
}

CObject::~CObject()
{
	SAFE_RELEASE(this->_texture);
}

void CObject::setAnchorPoint(VECTOR2 center)
{
	_anchorPoint.x = center.x*_width;
	_anchorPoint.y = center.y*_height;
}

void CObject::setPosition(float x, float y)
{
	_position.x = x;
	_position.y = y;
}


void CObject::setFlipX()
{
	_scale.x *= -1;
}

void CObject::setFlipY()
{
	_scale.y *= -1;
}

void CObject::setScale(float r1, float r2)
{
	_scale = ccp(r1, r2);
}

void CObject::setScaleX(float r)
{
	_scale.x = r;
}

void CObject::setScaleY(float r)
{
	_scale.y = r;
}

void CObject::setRotate(double d)
{
	_rotate = TO_RADIANS(d);
}

BoudingBox CObject::getBox()
{
	return BoudingBox(_position.x, _position.y,_width,_height,0,0);
}

VECTOR2 CObject::getPosition()
{
	return _position;
}

TEXTURE CObject::getTexture()
{
	return _texture;
}

float CObject::getWidth()
{
	return _width;
}

void CObject::draw()
{
	MATRIX matTransform;
	D3DXMatrixTransformation2D(
		&matTransform,
		&_anchorPoint, 0, &_scale,
		&_anchorPoint, (float)_rotate,
		&_position);
	SET_TRANSFORM(matTransform);

	DRAW(_texture, nullptr, &VECTOR3(_anchorPoint.x, _anchorPoint.y,0), nullptr, _color);
}


