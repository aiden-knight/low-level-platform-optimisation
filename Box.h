#pragma once

#include "ColliderObject.h"

class Box : public ColliderObject
{
public:
	void* operator new (size_t size);

	void drawMesh() { glutSolidCube(1.0); }
};

