#pragma once
#include "ColliderObject.h"

class Box : public ColliderObject
{
public:
	Box() : ColliderObject()
	{
		isBox = true;
	}
#ifdef _DEBUG
	void* operator new (size_t size);
#endif

	void drawMesh() { glutSolidCube(1.0); }
};

