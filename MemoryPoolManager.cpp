#include "MemoryPoolManager.h"
#include "MemoryPool.h"
#include "MemoryManager.h"
#include "globals.h"
#include <cstdlib>
#include "ColliderObject.h"
#include "Octree.h"
#include <new> // placement new
#include <map>
#include <functional>

namespace MemoryPoolManager
{
	namespace
	{
		MemoryPool* poolPtr = nullptr;

		constexpr size_t staticPoolCount = 3;

#ifdef _DEBUG
		constexpr size_t staticPoolSizes[staticPoolCount] = {
			sizeof(ColliderObject) + sizeof(MemoryManager::Header) + sizeof(MemoryManager::Footer),
			sizeof(Octree::Octant) + sizeof(MemoryManager::Header) + sizeof(MemoryManager::Footer),
			sizeof(std::function<void()>) + sizeof(MemoryManager::Header) + sizeof(MemoryManager::Footer)
		};
#else
		constexpr size_t staticPoolSizes[staticPoolCount] = {
			sizeof(ColliderObject),
			sizeof(Octree::Octant),
			sizeof(std::function<void()>)
		};
#endif // _DEBUG

		std::array<StaticMemoryPool*, staticPoolCount> staticPools;

		void* start = nullptr;
		void* end = nullptr;
	}

	char InitMemoryPools()
	{
		// allocate enough memory for all pools
		char* memory = (char*)std::malloc(sizeof(MemoryPool) + (staticPoolCount * sizeof(StaticMemoryPool)));
		poolPtr = new (memory) MemoryPool(chunkSize, chunkCount); // create dynamic pool

		// amount of chunks to allocate for static memory pools
		constexpr unsigned int chunkCounts[staticPoolCount] = {
			boxCount + sphereCount,

			// Equation for number of octants taken from wolfram, (1 << 3 * ... ) is compile time power of 8
			(1.0/7.0) * (-1 + (1 << (3*(1 + octreeDepth)))),
			maxTasks
		};

		// create static pools
		char* staticPoolBegin = memory + sizeof(MemoryPool);
		for (size_t i = 0; i < staticPoolCount; ++i)
		{
			staticPools[i] = new (staticPoolBegin + (i * sizeof(StaticMemoryPool))) StaticMemoryPool(staticPoolSizes[i], chunkCounts[i]);
		}

		return 0;
	}

	void* RequestMemory(size_t size)
	{
		static char initialised = InitMemoryPools();

		// if should be fit into static pool try and place it in there
		for (size_t i = 0; i < staticPoolCount; ++i)
		{
			if (staticPoolSizes[i] == size)
			{
				return staticPools[i]->Allocate();
			}
		}
		
		// otherwise try and place it in dynamic pool
		return poolPtr->Allocate(size);
	}

	bool FreeMemory(void* ptr)
	{
		// if pools already destroyed (aka after main)
		if (poolPtr == nullptr)
		{
			// returns whether it would have already been freed
			return ptr >= start && ptr <= end;
		}
		
		for (size_t i = 0; i < staticPoolCount; ++i)
		{
			if (staticPools[i]->Free(ptr))
			{
				return true;
			}
		}

		return poolPtr->Free(ptr);
	}

	void PrintPoolDebugInfo()
	{
		poolPtr->Print();
		for (size_t i = 0; i < staticPoolCount; ++i)
		{
			staticPools[i]->Print();
		}
	}

	void Cleanup()
	{
		if (poolPtr != nullptr)
		{
			start = poolPtr->start;
			end = poolPtr->end;

			poolPtr->~MemoryPool();
			for (size_t i = 0; i < staticPoolCount; ++i)
			{
				staticPools[i]->~StaticMemoryPool();
			}

			std::free(poolPtr);
			poolPtr = nullptr;
		}
	}
}
