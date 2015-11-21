#pragma once

#include <d3dx9.h>
#include <string>
#include "GameConfig.h"
#include "CGraphics.h"
#include "BoudingBox.h"

class CObject
{
protected:
	TEXTURE _texture;
	VECTOR2 _anchorPoint;
	VECTOR2 _scale;
	D3DCOLOR _color;
	double _rotate;
	VECTOR2 _position;
	float _width, _height;

public:
	CObject::CObject();
	CObject(std::string fileName);
	CObject(std::string fileName,int x,int y);
	CObject(int x, int y, int width, int height);
	CObject(std::string fileName,D3DCOLOR color);
	~CObject();

	void setAnchorPoint(VECTOR2 center);
	void setPosition(float x,float y);
	void setFlipX();
	void setFlipY();
	void setScale(float r1, float r2);
	void setScaleX(float r);
	void setScaleY(float r);
	void setRotate(double d);

	BoudingBox getBox();
	VECTOR2 getPosition();
	TEXTURE getTexture();
	float getWidth();

	void CObject::init(std::string fileName)
	{
		D3DXIMAGE_INFO info = CREATE_TEXTURE(fileName.c_str(), _texture);
		_width = (float)info.Width;
		_height = (float)info.Height;
		//_anchorPoint = ccp(float(_width / 2), float(_height / 2));
		//set anchor point at (bottom,left)
		_anchorPoint = ccp(0, 0);
		_rotate = 0.0f;
		_scale = ccp(1.0f, 1.0f);
		//_x = 0;
		//_y = 0;
		_color = D3DCOLOR_XRGB(255, 255, 255);
	}

	virtual void draw();

	void moveLeft()
	{
		_position.x -= 10;
	}

	void moveRight()
	{
		_position.y += 10;
	}
};