#pragma once
#ifdef _DEBUG
#include <cstdint>

namespace MemoryManager
{
	enum TrackerIndex : unsigned int;

	/// <summary>
	/// Header to track allocation size and relevant memory tracker 
	/// </summary>
	struct Header
	{
		size_t allocationSize;
		TrackerIndex trackerIndex;

		Header* next;
		Header* prev;

		uint32_t checkVal;

		Header(size_t size, TrackerIndex tracker);
	};

	struct Footer
	{
		uint32_t checkVal;

		Footer();
	};

	void OutputAllocations();
	void WalkHeap();
	void* UpdateTrackerDeallocation(void* ptr);
	const size_t GetAllocSize(const size_t requested);
	void* UpdateTrackerAllocation(void* ptr, const size_t size, const TrackerIndex tracker);
	void Cleanup();
}

#endif