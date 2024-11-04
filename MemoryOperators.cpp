#include "MemoryOperators.h"
#include "MemoryPoolManager.h"
#include <cstdlib>
#include <iostream>

#ifdef _DEBUG
#include "TrackerIndex.h"
#include "MemoryManager.h" 
#endif

/// <summary>
/// Global new for default memory allocation
/// </summary>
void* operator new(size_t size)
{
#ifdef _DEBUG
	return ::operator new(size, MemoryManager::TrackerIndex::Default);// calls global new with Default tracker index passed through
#else
	void* ptr = MemoryPoolManager::RequestMemory(size);
	if (ptr == nullptr)
	{
		std::cout << "Allocated using malloc not pool " << size << std::endl;
		ptr = std::malloc(size);
	}
	return ptr;
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

	// if memory was not in a memory pool free it manually
	if (!MemoryPoolManager::FreeMemory(ptr))
		std::free(ptr);
}

#ifdef _DEBUG
void* operator new(size_t size, MemoryManager::TrackerIndex tracker)
{
	const size_t allocSize = MemoryManager::GetAllocSize(size);
	void* ptr = MemoryPoolManager::RequestMemory(allocSize);
	if (ptr == nullptr)
	{
		ptr = std::malloc(allocSize);
		std::cout << "Allocated using malloc not pool " << size << std::endl;
	}
	
	return MemoryManager::UpdateTrackerAllocation(ptr, size, tracker);
}
#endif