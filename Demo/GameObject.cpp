#include "GameObject.h"



GameObject::GameObject()
{
	
}

GameObject::GameObject(int x, int y, int width, int height, int id, int type) :CObject(x, y, width, height), _id(id), _type(type), _group{}, _listPointCollide{} {}

GameObject::GameObject(int x, int y, int id, int type, std::string path) : CObject(path, x, y), _id(id), _type(type), _group{}, _listPointCollide{} {}

GameObject::~GameObject()
{
}

void GameObject::addSubObject(SubObject * subobj)
{
	_group.push_back(subobj);

}

void GameObject::addListSubObject(std::vector<SubObject*> group)
{
	std::copy(group.begin(), group.end(), std::back_inserter(_group));
}

void GameObject::addListVectorCollide(std::vector<std::pair<VECTOR2, VECTOR2>> listPointCollide)
{
	std::move(listPointCollide.begin(), listPointCollide.end(), std::back_inserter(_listPointCollide));
}

void GameObject::drawWithCamera(CCamera * cam)
{
	VECTOR3 pos(this->getPosition().x, this->getPosition().y, 0);

	VECTOR2 temp = cam->getPointTransform(pos.x, pos.y);

	VECTOR3 post(temp.x, cam->_position.y - temp.y, 0);

	DRAW(this->getTexture(), 0, 0, &post, D3DCOLOR_XRGB(255, 255, 255));
}

void SubObject::draw(CCamera* cam)
{
	if (_classify == ObjectClassify::single)
	{
		VECTOR3 pos(this->getPosition().x, this->getPosition().y, 0);

		VECTOR2 temp = cam->getPointTransform(pos.x, pos.y);

		VECTOR3 post(temp.x, cam->_position.y - temp.y, 0);

		DRAW(this->getTexture(), 0, 0, &post, D3DCOLOR_XRGB(255, 255, 255));
	}
	else
	{
		for (int i = 0; i < _n; ++i)
		{
			VECTOR3 pos(this->getPosition().x + (i*this->getWidth()), this->getPosition().y, 0);

			VECTOR2 temp = cam->getPointTransform(pos.x, pos.y);

			VECTOR3 post(temp.x, cam->_position.y - temp.y, 0);

			DRAW(this->getTexture(), 0, 0, &post, D3DCOLOR_XRGB(255, 255, 255));
		}
	}
}
