#pragma once

#include <vector>
#include <list>
#include <iterator>
#include <algorithm>
#include <Windows.h>
#include <d3dx9.h>
#include "BoudingBox.h"
#include "GameConfig.h"


class NodeObject
{
public:

	int _x, _y, _width, _height;
	int _objectID;
										
	NodeObject(int x, int y, int width, int height, int id) :_x(x), _y(y), _width(width), _height(height),_objectID(id) {}
	
};


class QuadNode
{
public:
	QuadNode();
	QuadNode(int id, int x, int y, int width, int height, std::list<NodeObject*> listNodeObject);
	~QuadNode();

	QuadNode *_topLeft{}, *_topRight{}, *_bottomLeft{}, *_bottomRight{};
	int _id;
	BoudingBox _bouding;

	std::list<NodeObject*> _listNodeObject;
	int getParrentID();
};

class QuadTree
{
public:
	QuadTree();
	QuadTree(QuadNode* root);
	QuadNode* _root;

	std::list<NodeObject*> getListObjectInViewPort(BoudingBox viewport);

private:

	void traverseTree(QuadNode* node, BoudingBox viewport, std::list<NodeObject*>& result);
};


