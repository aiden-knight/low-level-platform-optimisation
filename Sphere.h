#pragma once
#include "ColliderObject.h"

class Sphere :
    public ColliderObject
{
public:
#ifdef _DEBUG
	void* operator new (size_t size);
#endif

    void drawMesh() { glutSolidSphere(0.5, 5, 5); }
}; 

