#pragma once
#include "ColliderObject.h"
#include "MemoryOperators.h"
#include "TrackerIndex.h"

class Box : public ColliderObject
{
public:
#ifdef _DEBUG
	void* operator new (size_t size)
	{
		return ::operator new(size, MemoryManager::TrackerIndex::Box);
	}
#endif

	void drawMesh() { glutSolidCube(1.0); }
};

