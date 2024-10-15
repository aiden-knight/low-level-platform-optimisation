#include "MemoryPoolManager.h"
#include "MemoryPool.h"

namespace MemoryPoolManager
{
	namespace
	{
		MemoryPool<100>* poolPtr;
	}

	char InitMemoryPools()
	{
		 static MemoryPool<100> pool { 64 };
		 poolPtr = &pool;
	}

	void* RequestMemory(size_t size)
	{
		static char initialised = InitMemoryPools();

		return poolPtr->Allocate(size);
	}

	bool FreeMemory(void* ptr)
	{
		return poolPtr->Free(ptr);
	}
}
