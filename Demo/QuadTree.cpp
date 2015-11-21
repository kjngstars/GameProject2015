#include "QuadTree.h"


QuadNode::QuadNode(int id, int x, int y, int width, int height, std::list<NodeObject*> listNodeObject)
{
	_id = id;
	_bouding._x = x;
	_bouding._y = y;
	_bouding._w = width;
	_bouding._h = height;
	

	std::move(listNodeObject.cbegin(), listNodeObject.cend(), std::back_inserter(_listNodeObject));
}

int QuadNode::getParrentID()
{
	return _id / 8;
}

QuadTree::QuadTree()
{
}

QuadTree::QuadTree(QuadNode * root)
{
	_root = root;
}

std::list<NodeObject*> QuadTree::getListObjectInViewPort(BoudingBox viewport)
{
	std::list<NodeObject*> result;

	traverseTree(_root, viewport, result);

	result.sort([](NodeObject* node1,NodeObject* node2) {
		return node1->_objectID < node2->_objectID;
	});

	//resolve duplicates object;
	auto pos = std::unique(result.begin(), result.end(),[](NodeObject* id1,NodeObject* id2) {
		return id1->_objectID == id2->_objectID;
	});

	result.erase(pos, result.end());
	
	return result;

}

void QuadTree::traverseTree(QuadNode * node, BoudingBox viewport, std::list<NodeObject*>& result)
{
	if (node == nullptr)
	{
		return;
	}

	if (node->_listNodeObject.size() != 0 && viewport.Intersect(node->_bouding))
	{
		std::copy(node->_listNodeObject.begin(), node->_listNodeObject.end(), std::back_inserter(result));
	}
	else if (!viewport.Intersect(node->_bouding))
	{
		return;
	}

	//traverse child node
	traverseTree(node->_topLeft, viewport, result);
	traverseTree(node->_topRight, viewport, result);
	traverseTree(node->_bottomLeft, viewport, result);
	traverseTree(node->_bottomRight, viewport, result);
}

