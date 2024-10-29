#include "MemoryPoolManager.h"
#include "MemoryPool.h"
#include "globals.h"
#include <cstdlib>
#include "ColliderObject.h"
#include <new> // placement new

namespace MemoryPoolManager
{
	namespace
	{
		MemoryPool* poolPtr = nullptr;
		StaticMemoryPool* staticPoolPtr = nullptr;
		size_t colliderAllocSize;
	}

	char InitMemoryPools()
	{
		char* memory = (char*)std::malloc(sizeof(MemoryPool) + sizeof(StaticMemoryPool));
		poolPtr = new (memory) MemoryPool(chunkSize, chunkCount);

		colliderAllocSize = sizeof(ColliderObject);
#ifdef _DEBUG
		colliderAllocSize += 24; // sizeof header and footer
#endif // _DEBUG

		staticPoolPtr = new (memory + sizeof(MemoryPool)) StaticMemoryPool(colliderAllocSize, boxCount + sphereCount);

		return 0;
	}

	void* RequestMemory(size_t size)
	{
		static char initialised = InitMemoryPools();

		if (size > 4 * chunkSize) return nullptr;

		if (size == colliderAllocSize)
		{
			return staticPoolPtr->Allocate();
		}
		else
		{
			return poolPtr->Allocate(size);
		}
	}

	bool FreeMemory(void* ptr)
	{
		bool freed = false;
		if (!freed && staticPoolPtr != nullptr)
		{
			freed = staticPoolPtr->Free(ptr);
		}
		if (!freed && poolPtr != nullptr)
		{
			freed = poolPtr->Free(ptr);
		}
		return freed;
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

		if (staticPoolPtr != nullptr)
		{
			staticPoolPtr->~StaticMemoryPool();
			std::free(staticPoolPtr);
			staticPoolPtr = nullptr;
		}
	}
}
