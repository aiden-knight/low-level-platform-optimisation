#include "MemoryOperators.h"
#include "MemoryManager.h" 
#include <cstdlib>

#ifdef _DEBUG
#include "TrackerIndex.h"
#endif

/// <summary>
/// Global new for default memory allocation
/// </summary>
void* operator new(size_t size)
{
#ifdef _DEBUG
	return ::operator new(size, MemoryManager::TrackerIndex::Default);// calls global new with Default tracker index passed through
#else
	return std::malloc(size);
#endif // _DEBUG
}

/// <summary>
/// Global delete overloaded to ensure header and footer data is cleaned up
/// </summary>
/// <param name="ptr"></param>
void operator delete(void* ptr)
{
	if (ptr == nullptr) return;

#ifdef _DEBUG
	ptr = MemoryManager::UpdateTrackerDeallocation(ptr);
#endif // _DEBUG

	std::free(ptr);
}

#ifdef _DEBUG
void* operator new(size_t size, MemoryManager::TrackerIndex tracker)
{
	void* ptr = std::malloc(MemoryManager::GetAllocSize(size));
	return MemoryManager::UpdateTrackerAllocation(ptr, size, tracker);
}
#endif