#pragma once
#include "ColliderObject.h"
#include "MemoryOperators.h"

class Sphere :
    public ColliderObject
{
public:
#ifdef _DEBUG
	void* operator new (size_t size)
	{
		return ::operator new(size, MemoryManager::Sphere);
	}
#endif

    void drawMesh() { glutSolidSphere(0.5, 5, 5); }
}; 

