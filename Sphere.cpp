#include "Sphere.h"
#include "MemoryOperators.h"
#include "TrackerIndex.h"

#ifdef _DEBUG
void* Sphere::operator new(size_t size)
{
	return ::operator new(size, MemoryManager::TrackerIndex::Sphere);
}
#endif