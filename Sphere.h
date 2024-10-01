#pragma once
#include "ColliderObject.h"
class Sphere :
    public ColliderObject
{
public:
    void* operator new(size_t size);

    void drawMesh() { glutSolidSphere(0.5, 5, 5); }
}; 

