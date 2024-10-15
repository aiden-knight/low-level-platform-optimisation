#pragma once
// global operators
void* operator new(size_t size);
void operator delete(void* ptr);

#ifdef _DEBUG
namespace MemoryManager
{
	enum TrackerIndex : unsigned int;
}
void* operator new(size_t size, MemoryManager::TrackerIndex tracker);
#endif