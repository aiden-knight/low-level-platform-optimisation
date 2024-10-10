#pragma once
#ifdef _DEBUG
#include "TrackerIndex.h"

namespace MemoryManager
{
	void OutputAllocations();
	void WalkHeap();
}

// global operators
void* operator new(size_t size);
void operator delete(void* ptr);

void* operator new(size_t size, MemoryManager::TrackerIndex tracker);

#endif