#pragma once

#include <regex>
#include <fstream>
#include <string>
#include <numeric>
#include <list>
#include <vector>
#include <set>
#include <map>
#include <algorithm>
#include <functional>
#include <cassert>
#include <d3d9.h>
#include "GameConfig.h"
#include "CGraphics.h"
#include "Camera.h"
#include "CObject.h"
#include "GameObject.h"
#include "QuadTree.h"
#include "xmlparser\tinyxml2.h"
#include "BoudingBox.h"

using namespace tinyxml2;
using namespace std::placeholders;

struct Tile
{
	int _xMap, _yMap;
	int _index;

	Tile(int xMap, int yMap, int index)
	{
		_xMap = xMap;
		_yMap = yMap;
		_index = index;
	}
};

struct EnemyInfo
{
	int _id;
	int _type;
	int _x, _y;
	int _patrolWidth, _patrolHeight;
	int _direction;

	EnemyInfo(int id,int type, int x, int y, int patrolwidth, int patrolheight, int direction) :
		_id(id),
		_type(type),
		_x(x), _y(y),
		_patrolWidth(patrolwidth), _patrolHeight(patrolheight),
		_direction(direction) {}
};

class CMap
{
public:

	CMap(std::string fileMap, std::string fileTileSet);
	CMap();
	void init(std::string fileMap, std::string fileTileSet);
	void loadMapObject(std::string fileMapObject);
	void loadQuadTree(std::string path);
	void loadEnemies(std::string path);
	void drawBackground();
	void drawObject();

	std::list<NodeObject*> getListNodeObjectInViewPort();
	std::list<GameObject*> getListGameObjectInViewPort();
	std::map<int, GameObject*> getMapObject();
	std::list<EnemyInfo> getListEnemiesInfo();

	void getMatchedNodeObject(NodeObject* o);

	CCamera* getCamera();

	//auxilary function
	std::string getText(XMLElement* element, std::string tag);
	std::pair<int, int> parseElement(std::string str);

	float GetMapWidthSize() { return this->_mapWidth*this->_mapSize*this->_tileSize; }
	float GetMapHeightSize() { return this->_mapHeight*this->_tileSize; }
	std::vector<std::pair<D3DXVECTOR2, D3DXVECTOR2>> GetListLine()
	{
		return this->listLine;
	}

	std::vector<std::pair<D3DXVECTOR2, D3DXVECTOR2>> GetListLineWithoutQuadtree();
	std::vector<std::pair<D3DXVECTOR2, D3DXVECTOR2>> GetListLineWithQuadtree();

	std::list<EnemyInfo> CMap::getListEnemyInViewPort();

private:
	TEXTURE _tileSet;
	std::vector<int> _tileList;
	std::set<int> _tileIndex;
	std::map<int, GameObject*> _mapObject;
	std::map<int, QuadNode*> _listQuadNode;
	std::vector<Tile> _listTileMap;
	std::list<GameObject*> _listGameObjectInViewPort;
	std::list<EnemyInfo> _listEnemiesInfo;
	int _mapWidth;
	int _mapHeight;
	int _tileSize;
	int _mapSize;

	std::vector<std::pair<D3DXVECTOR2, D3DXVECTOR2>> listLine;

	QuadTree* _quadTree;

	CCamera* _camera;

	//draw invidual game object
	void drawGameObject(GameObject* obj, CCamera* cam);

	void makeQuadTree(std::map<int, QuadNode*> listQuadNode);
};


