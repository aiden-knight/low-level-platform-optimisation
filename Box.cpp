#include "Box.h"
#include "MemoryOperators.h"
#include "TrackerIndex.h"

#ifdef _DEBUG
void* Box::operator new(size_t size)
{
	return ::operator new(size, MemoryManager::TrackerIndex::Box);
}
#endif