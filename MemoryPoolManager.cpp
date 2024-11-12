#include "MemoryPoolManager.h"
#include "MemoryPool.h"
#include "MemoryManager.h"
#include "globals.h"
#include <cstdlib>
#include "ColliderObject.h"
#include "Octree.h"
#include <new> // placement new
#include <map>
#include <queue>


namespace MemoryPoolManager
{
	namespace
	{
		MemoryPool* poolPtr = nullptr;

		constexpr size_t staticPoolCount = 3;
		constexpr size_t octantQueueBlockSize = std::queue<Octree::Octant*>::container_type::_EEN_DS;

#ifdef _DEBUG
		constexpr size_t staticPoolSizes[staticPoolCount] = {
			sizeof(ColliderObject) + sizeof(MemoryManager::Header) + sizeof(MemoryManager::Footer),
			sizeof(Octree::Octant) + sizeof(MemoryManager::Header) + sizeof(MemoryManager::Footer),
			(octantQueueBlockSize * sizeof(Octree::Octant*)) + sizeof(MemoryManager::Header) + sizeof(MemoryManager::Footer)
		};
#else
		constexpr size_t staticPoolSizes[staticPoolCount] = {
			sizeof(ColliderObject),
			sizeof(Octree::Octant),
			octantQueueBlockSize * sizeof(Octree::Octant*)
		};
#endif // _DEBUG

		std::array<StaticMemoryPool*, staticPoolCount> staticPools;

		void* start = nullptr;
		void* end = nullptr;
	}

	char InitMemoryPools()
	{
		if (poolPtr) return 1;

		// allocate enough memory for all pools
		char* memory = (char*)std::malloc(sizeof(MemoryPool) + (staticPoolCount * sizeof(StaticMemoryPool)));
		poolPtr = new (memory) MemoryPool(chunkSize, chunkCount); // create dynamic pool

		return 0;
	}

	void Init()
	{
		if (!poolPtr)
		{
			InitMemoryPools();
		}

		// amount of chunks to allocate for static memory pools
		unsigned int chunkCounts[staticPoolCount] = {
			boxCount + sphereCount,

			// Equation for number of octants taken from wolfram, (1 << 3 * ... ) is compile time power of 8
			(1.0 / 7.0) * (-1 + (1 << (3 * (1 + octreeDepth)))),
			((1.0 / 7.0) * (-1 + (1 << (3 * (1 + octreeDepth)))) / octantQueueBlockSize) + 3
		};

		// create static pools
		char* staticPoolBegin = (char*)poolPtr + sizeof(MemoryPool);
		for (size_t i = 0; i < staticPoolCount; ++i)
		{
			staticPools[i] = new (staticPoolBegin + (i * sizeof(StaticMemoryPool))) StaticMemoryPool(staticPoolSizes[i], chunkCounts[i]);
		}
	}

	void* RequestMemory(size_t size)
	{
		static char initialised = InitMemoryPools();

		// if should be fit into static pool try and place it in there
		for (size_t i = 0; i < staticPoolCount; ++i)
		{
			if (staticPools[i] && staticPoolSizes[i] == size)
			{
				return staticPools[i]->Allocate();
			}
		}
		
		// otherwise try and place it in dynamic pool
		if (size > (chunkSize * 4)) return nullptr;
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
