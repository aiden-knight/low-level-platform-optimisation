#pragma once
#ifdef _DEBUG

namespace MemoryManager
{
	enum TrackerIndex : unsigned int;

	void OutputAllocations();
	void WalkHeap();
	void* UpdateTrackerDeallocation(void* ptr);
	const size_t GetAllocSize(const size_t requested);
	void* UpdateTrackerAllocation(void* ptr, const size_t size, const TrackerIndex tracker);
	void Cleanup();
}

#endif