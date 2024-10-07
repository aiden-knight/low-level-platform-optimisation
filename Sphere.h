#pragma once
#include "ColliderObject.h"
#include "MemoryManager.h"

class Sphere :
    public ColliderObject
{
public:
#ifdef _DEBUG
	void* operator new (size_t size)
	{
		return ::operator new(size, SPHERE);
	}
#endif

    void drawMesh() { glutSolidSphere(0.5, 5, 5); }
}; 

