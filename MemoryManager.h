#pragma once
#ifdef _DEBUG
#include "Tracker.h"

namespace MemoryManager
{
	void OutputAllocations();
}

// global operators
void* operator new(size_t size);
void operator delete(void* ptr);

void* operator new(size_t size, MemoryManager::TrackerIndex tracker);

#endif