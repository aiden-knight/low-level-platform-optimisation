#include "MemoryPoolManager.h"
#include "MemoryPool.h"
#include <cstdlib>
#include <iostream>

namespace MemoryPoolManager
{
	namespace
	{
		MemoryPool* poolPtr = nullptr;
	}

	char InitMemoryPools()
	{
		void* memory = std::malloc(sizeof(MemoryPool));
		constexpr size_t chunkSize = 100;
		constexpr size_t chunkCount = 109;
		poolPtr = new (memory) MemoryPool(chunkSize, chunkCount);

		return 0;
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

	void PrintPoolDebugInfo()
	{
		poolPtr->Print();
	}

	void Cleanup()
	{
		if (poolPtr != nullptr)
		{
			poolPtr->~MemoryPool();
			std::free(poolPtr);
			poolPtr = nullptr;
		}
	}
}
