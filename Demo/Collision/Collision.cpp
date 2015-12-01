#include "Collision.h"
#include <math.h>

float SweptAABB(float elapsedTime, BoudingBox box1, BoudingBox box2, float& normalX, float& normalY)
{
	float xInvEntry, yInvEntry;
	float xInvExit, yInvExit;

	if (box1._vx > 0.0f)
	{
		xInvEntry = box2._x - (box1._x + box1._w);
		xInvExit = (box2._x + box2._w) - box1._x;
	}
	else
	{
		xInvEntry = (box2._x + box2._w) - box1._x;
		xInvExit = box2._x - (box1._x + box1._w);
	}

	if (box1._vy > 0.0f)
	{
		yInvEntry = (box2._y - box2._h) - box1._y;
		yInvExit = box2._y - (box1._y - box1._h);
	}
	else
	{
		yInvEntry = box2._y - (box1._y - box1._h);
		yInvExit = (box2._y - box2._h) - box1._y;
	}

	float xEntry, yEntry;
	float xExit, yExit;

	if (box1._vx == 0.0f)
	{
		if (box1._vy != 0.0f)
		{
			float minX = fmin(box1._x, box2._x);
			float maxX = fmax(box1._x + box1._w, box2._x + box2._w);

			if (maxX - minX > box1._w + box2._w)
			{
				normalX = 0.0f;
				normalY = 0.0f;
				return elapsedTime;
			}
		}

		xEntry = -std::numeric_limits<float>::infinity();
		xExit = std::numeric_limits<float>::infinity();
	}
	else
	{
		xEntry = xInvEntry / box1._vx;
		xExit = xInvExit / box1._vx;
	}

	if (box1._vy == 0.0f)
	{
		if (box1._vx != 0.0f)
		{
			float minY = fmin(box1._y - box1._h, box2._y - box2._h);
			float maxY = fmax(box1._y, box2._y);

			if (maxY - minY > box1._h + box2._h)
			{
				normalX = 0.0f;
				normalY = 0.0f;
				return elapsedTime;
			}
		}

		yEntry = -std::numeric_limits<float>::infinity();
		yExit = std::numeric_limits<float>::infinity();
	}
	else
	{
		yEntry = yInvEntry / box1._vy;
		yExit = yInvExit / box1._vy;
	}

	float entryTime = max(xEntry, yEntry);
	float exitTime = min(xExit, yExit);

	if (entryTime > exitTime || xEntry < 0.0f && yEntry < 0.0f ||
		xEntry > elapsedTime || yEntry > elapsedTime)
	{
		normalX = 0.0f;
		normalY = 0.0f;
		return elapsedTime;
	}
	else
	{
		if (xEntry > yEntry)
		{
			if (xInvEntry < 0.0f)
			{
				normalX = 1.0f;
				normalY = 0.0f;
			}
			else
			{
				normalX = -1.0f;
				normalY = 0.0f;
			}
		}
		else
		{
			if (yInvEntry < 0.0f)
			{
				normalX = 0.0f;
				normalY = -1.0f;
			}
			else
			{
				normalX = 0.0f;
				normalY = 1.0f;
			}
		}

		return entryTime;
	}
}