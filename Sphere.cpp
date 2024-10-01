#include "Sphere.h"
#include "MemoryManager.h"

void* Sphere::operator new(size_t size)
{
    return ::operator new(size, SPHERE);
}
