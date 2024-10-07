#pragma once
#include "ColliderObject.h"
#include "MemoryManager.h"

class Box : public ColliderObject
{
public:
#ifdef _DEBUG
	void* operator new (size_t size)
	{
		return ::operator new(size, BOX);
	}
#endif

	void drawMesh() { glutSolidCube(1.0); }
};

