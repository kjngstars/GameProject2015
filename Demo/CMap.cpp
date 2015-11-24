#include "CMap.h"
#include "CGame.h"



CMap::CMap() :_tileSize(64)
{		
}

void CMap::init(std::string fileMap, std::string fileTileSet)
{
	//load tileset
	CREATE_TEXTURE(fileTileSet.c_str(), _tileSet);

	std::ifstream reader(fileMap);
	reader.ignore(256, '\n');
	std::string firstLine{}, content{}, line{};

	//read first line
	std::getline(reader, firstLine);

	//read the rest
	while (std::getline(reader, line))
	{
		content += line;
	}
	
	//parse content
	std::smatch m;
	std::regex reg1("<.+ w=\"([0-9]+)\" h=\"([0-9]+)\" s=\"([0-9]+)\">");
	std::regex reg2("<[^>]*\"([0-9]+)\" />");

	//parse first line
	bool found = std::regex_search(firstLine.cbegin(), firstLine.cend(), m, reg1);
	assert(found);
	_mapWidth = std::stoi(m.str(1));
	_mapHeight = std::stoi(m.str(2));
	_mapSize = std::stoi(m.str(3));
	//rest
	auto pos = content.cbegin();

	for (; std::regex_search(pos, content.cend(), m, reg2); pos = m.suffix().first)
	{
		_tileList.push_back(std::stoi(m[1].str()));
	}

	//
	for (int i = 0; i < _tileList.size(); i++)
	{
		_tileIndex.insert(_tileList.at(i));
	}
	
	//camera
	_camera = new CCamera(0, _mapWidth*_mapSize * 64);	

	//add to list tile map 4 drawing
	
	int index{};
	int count{};

	for (int j = 0; j <= _mapSize; j++)
	{

		for (size_t i = 0; i < _tileList.size(); ++i)
		{
			int x = (((i%_mapWidth) * _tileSize) + (_mapWidth*_tileSize)*j);
			int y = ((i / _mapWidth)*_tileSize) - 232;
			auto pos = std::find(_tileIndex.cbegin(), _tileIndex.cend(), _tileList.at(i));
			index = (std::distance(_tileIndex.cbegin(), pos));

			_listTileMap.push_back(Tile(x, y, index));
		}
	}

}

void CMap::loadMapObject(std::string fileMapObject)
{
	XMLError result;
	tinyxml2::XMLDocument xmlDoc;
	xmlDoc.LoadFile(fileMapObject.c_str());

	XMLNode* root = xmlDoc.FirstChildElement("mapobject");
	//XMLNode* root = xmlDoc.RootElement();

	if (root == nullptr)
	{		
		MessageBox(0, "Invalid file map", 0, 0);
		PostQuitMessage(0);
	}

	XMLElement* pElement = root->FirstChildElement("object");

	while (pElement != nullptr)
	{
		std::vector<std::pair<VECTOR2, VECTOR2>> listPointCollide;
		int id;
		const char* category = nullptr;
		const char* text = nullptr;
		XMLElement* childElement = nullptr;
		XMLElement* lines = nullptr;
		XMLElement* line = nullptr;

		//parse attribute: id
		result = pElement->QueryIntAttribute("id", &id);

		if (result != XMLError::XML_NO_ERROR)
		{			
			break;
		}

		category = pElement->Attribute("category");

		if (category == nullptr)
		{			
			break;
		}				

		std::string str(category);

		lines = pElement->FirstChildElement("lines");
		if (lines != nullptr)
		{
			line = lines->FirstChildElement("line");
			while (line != nullptr)
			{
				std::string p1 = getText(line, "p1");
				std::string p2 = getText(line, "p2");

				auto point1 = parseElement(p1);
				auto point2 = parseElement(p2);

				//add to list point collide
				listPointCollide.push_back(std::make_pair(VECTOR2(point1.first, point1.second), VECTOR2(point2.first, point2.second)));

				line = line->NextSiblingElement("line");

			}
		}

		//case blockS
		if (str == "block")
		{
			std::vector<SubObject*> group;
			

			//parse two first child element
			childElement = pElement->FirstChildElement("pos");
			text = childElement->GetText();
			std::string position(text);

			childElement = pElement->FirstChildElement("size");
			text = childElement->GetText();
			std::string size(text);

			//parse the rest sub-object
			XMLElement* subObject = nullptr;
			
			subObject = pElement->FirstChildElement("subobj");

			while (subObject != nullptr)
			{
				std::string pos, size, type;
				
				XMLElement *seqTag = nullptr;
				seqTag = subObject->FirstChildElement("seq");

				if (seqTag != nullptr)
				{
					std::string n = seqTag->Attribute("n");
										
					type = getText(seqTag, "t");
					pos = getText(seqTag, "pos");
					size = getText(seqTag, "size");
										
					auto p = parseElement(pos);
					
					std::string path = CGame::_listPathObject[std::stoi(type)];
					SubObject* subObj = new SubObject(p.first, p.second - 232, std::stoi(type), std::stoi(n), SubObject::ObjectClassify::sequences, path);
					group.push_back(subObj);

				}
				else
				{
					type = getText(subObject, "t");
					pos = getText(subObject, "pos");
					size = getText(subObject, "size");
					
					auto p = parseElement(pos);

					std::string path = CGame::_listPathObject[std::stoi(type)];
					SubObject* subObj = new SubObject(p.first, p.second - 232, std::stoi(type), 1, SubObject::ObjectClassify::single, path);
					group.push_back(subObj);
				}

				//go to next sibling
				subObject = subObject->NextSiblingElement("subobj");
			}

			//get position
			auto pairPos = parseElement(position);
			auto pairSize = parseElement(size);

			//add to list game object
			GameObject* gameObject = new GameObject(pairPos.first, pairPos.second - 232, pairSize.first, pairSize.second, id, -1);
			gameObject->addListSubObject(group);
			gameObject->addListVectorCollide(listPointCollide);
			_mapObject.insert(std::pair<int, GameObject*>(id, gameObject));
		}
		else if (str == "single") //case single
		{
			std::string pos, size, type;
			type = getText(pElement, "t");
			pos = getText(pElement, "pos");
			size = getText(pElement, "size");

			//add to list game object
			std::string path = CGame::_listPathObject[std::stoi(type)];
			auto pairPos = parseElement(pos);
			GameObject* gameObject = new GameObject(pairPos.first, pairPos.second - 232, id, std::stoi(type), path);
			_mapObject.insert(std::pair<int, GameObject*>(id, gameObject));
		}

		//go to next sibling
		pElement = pElement->NextSiblingElement("object");
	}
}

void CMap::drawBackground()
{
	
	for (auto& tile : _listTileMap)
	{
		RECT r;
		VECTOR2 temp = _camera->getPointTransform((float)tile._xMap, (float)tile._yMap);
		VECTOR3 pos(temp.x, _camera->_position.y - temp.y, 0);

		r.left = tile._index*_tileSize;
		r.top = 0;
		r.right = _tileSize + r.left;
		r.bottom = _tileSize + r.top;

		DRAW(_tileSet, &r, nullptr, &pos, D3DCOLOR_XRGB(255, 255, 255));
	}
			
}

void CMap::drawObject()
{	
	_listGameObjectInViewPort.clear();

	auto listNodeObjectInViewPort = this->getLisNodetObjectInViewPort();
	
	/*for (auto& i : _mapObject)
	{
		this->drawGameObject(i.second,_camera);
	}*/
	
	std::for_each(listNodeObjectInViewPort.cbegin(), listNodeObjectInViewPort.cend(), 
				 std::bind(&CMap::getMatchedNodeObject, this, _1));
	
	for (auto& i : _listGameObjectInViewPort)
	{
	this->drawGameObject(i,_camera);
	}

}

std::list<NodeObject*> CMap::getLisNodetObjectInViewPort()
{
	BoudingBox viewport;
	viewport._x = _camera->_position.x;
	viewport._y = _camera->_position.y;
	viewport._w = SCREEN_WIDTH;
	viewport._h = SCREEN_HEIGHT;

	return _quadTree->getListObjectInViewPort(viewport);
}

std::list<GameObject*> CMap::getListGameObjectInViewPort()
{
	return _listGameObjectInViewPort;
}

std::map<int, GameObject*> CMap::getMapObject()
{
	return _mapObject;
}

std::list<EnemyInfo> CMap::getListEnemiesInfo()
{
	return _listEnemiesInfo;
}

void CMap::getMatchedNodeObject(NodeObject * o)
{
	auto matched = _mapObject[o->_objectID];
	_listGameObjectInViewPort.push_back(matched);
}


CCamera * CMap::getCamera()
{
	return _camera;
}

std::string CMap::getText(XMLElement * element, std::string tag)
{
	XMLElement* child = nullptr;
	if (element == nullptr)
	{
		//cout << "invalid argument" << endl;
		return nullptr;
	}

	child = element->FirstChildElement(tag.c_str());
	if (child == nullptr)
	{
		//cout << "invalid tag" << endl;
		return nullptr;
	}

	std::string text = child->GetText();

	return text;
}

std::pair<int, int> CMap::parseElement(std::string str)
{
	int commaIndex = str.find(',');
	std::string xw = str.substr(1, commaIndex - 1);
	
	int closeBraceIndex = str.find('}');
	std::string yh = str.substr(commaIndex + 1, closeBraceIndex - commaIndex - 1);

	return std::pair<int, int>(std::stoi(xw), std::stoi(yh));
}

void CMap::drawGameObject(GameObject* obj, CCamera* cam)
{
	if (obj->getType() == -1)
	{
		auto g = obj->getGroupObject();
		for (auto& i : g)
		{
			i->draw(cam);
		}
	}
	else
	{
		obj->drawWithCamera(cam);
	}
}

void CMap::makeQuadTree(std::map<int, QuadNode*> listQuadNode)
{
	for (auto& i : listQuadNode)
	{
		//if root node
		if (i.first == 0)
		{
			//init tree with root node
			_quadTree = new QuadTree(i.second);
		}
		else
		{	
			//get parrent id
			int parentID = i.second->getParrentID();

			auto parrentNode = listQuadNode[parentID];

			if (parrentNode == nullptr)
			{
				continue;
			}

			//calculate child position
			int childPos = i.first % 8;

			switch (childPos)
			{
			case 1:
				parrentNode->_topLeft = i.second;
				break;
			case 2:
				parrentNode->_topRight = i.second;
				break;
			case 3:
				parrentNode->_bottomRight = i.second;
				break;
			case 4:
				parrentNode->_bottomLeft = i.second;
				break;
			}
		}
	}
}

void CMap::loadQuadTree(std::string path)
{
	
	tinyxml2::XMLDocument xmlDoc;
	xmlDoc.LoadFile(path.c_str());

	XMLNode* root = xmlDoc.FirstChildElement("quadtree");

	if (root == nullptr)
	{
		MessageBox(0, "Invalid file quadtree", 0, 0);
		PostQuitMessage(0);
	}

	XMLElement* pElement = root->FirstChildElement("node");

	while (pElement != nullptr)
	{
		int nodeID;
		std::string pos, size;
		std::list<NodeObject*> listNodeObject;

		pElement->QueryIntAttribute("id", &nodeID);
		pos = getText(pElement, "pos");
		size = getText(pElement, "size");

		XMLElement* listObj = pElement->FirstChildElement("list-object");				

		if (listObj != nullptr)
		{
			XMLElement* idElement = listObj->FirstChildElement("id");

			while (idElement != nullptr)
			{
				std::string text = idElement->GetText();
				
				//create node objct
				auto obj = _mapObject[std::stoi(text)];
				auto box = obj->getBox();
				NodeObject* nodeObject = new NodeObject(box._x, box._y, box._w, box._h, std::stoi(text));
				listNodeObject.push_back(nodeObject);

				//go to next sibling
				idElement = idElement->NextSiblingElement("id");
			}
						
		}

		//add to list quadnode			

		//get position and size of node
		auto t1 = parseElement(pos);
		auto t2 = parseElement(size);

		_listQuadNode.insert(std::pair<int, QuadNode*>(nodeID, new QuadNode(nodeID, t1.first, t1.second, t2.first, t2.second, listNodeObject)));;

		//go to next sibling ("node")
		pElement = pElement->NextSiblingElement("node");
	}

	//make quadtree
	this->makeQuadTree(_listQuadNode);

}

void CMap::loadEnemies(std::string path)
{
	tinyxml2::XMLDocument doc;
	doc.LoadFile(path.c_str());

	XMLNode* root = doc.FirstChildElement("enemies");

	if (root == nullptr)
	{
		MessageBox(0, "Invalid file quadtree", 0, 0);
		PostQuitMessage(0);
	}

	XMLElement* pElement = root->FirstChildElement("enemy");

	while (pElement != nullptr)
	{
		int id;
		pElement->QueryIntAttribute("id", &id);

		std::string type = getText(pElement, "t");
		std::string pos = getText(pElement, "pos");
		std::string patrolwidth = getText(pElement, "patrolwidth");
		std::string patrolheight = getText(pElement, "patrolheight");
		std::string direction = getText(pElement, "direction");

		pElement = pElement->NextSiblingElement("enemy");
		
		auto position = parseElement(pos);
		
		_listEnemiesInfo.push_back(EnemyInfo(id, std::stoi(type), position.first, position.second, std::stoi(patrolwidth), std::stoi(patrolheight), std::stoi(direction)));
	}
}


