#pragma once

#include <vector>
#include "CObject.h"
#include "Camera.h"

class SubObject;

class GameObject : public CObject
{
public:
	GameObject();
	GameObject(int x, int y, int width, int height, int id, int type);
	GameObject(int x, int y, int id, int type, std::string path);
	~GameObject();
	
	void addSubObject(SubObject* subobj);
	void addListSubObject(std::vector< SubObject* > group);
	void addListVectorCollide(std::vector<std::pair<VECTOR2, VECTOR2>> listPointCollide);

	int getType()
	{
		return _type;
	}

	int getID()
	{
		return _id;
	}

	std::vector<SubObject*> getGroupObject()
	{
		return _group;
	}

	std::vector<std::pair<VECTOR2, VECTOR2>> getListPointCollide()
	{
		return _listPointCollide;
	}

	void drawWithCamera(CCamera* cam);

private:
	int _id;
	int _type;

	//if you don't well-known about initilize list, don't use it, otherwise it will lead to awkward confusing error
	//std::vector<SubObject*> _group{};
	std::vector<SubObject*> _group;
	std::vector<std::pair<VECTOR2,VECTOR2>> _listPointCollide;
};

class SubObject :public CObject
{
public:
	enum ObjectClassify
	{
		single = 1,
		sequences
	};		
	int _n;
	int _type;
	ObjectClassify _classify;

	SubObject() {}
	SubObject(int x, int y, int type, int n, ObjectClassify classify, std::string path) : CObject(path, x, y), _n(n), _type(type), _classify(classify) {}
	
	virtual void draw(CCamera* cam);
};

