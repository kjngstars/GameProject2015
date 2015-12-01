#pragma once

#include "../BoudingBox.h"
#include <limits>

float SweptAABB(float elapsedTime, BoudingBox box1, BoudingBox box2, float& normalX, float& normalY);