#pragma once
#ifdef _DEBUG
enum TrackerIndex : unsigned int;

namespace MemoryManager
{
	void OutputAllocations();
	void WalkHeap();
	void* UpdateTrackerDeallocation(void* ptr);
	const size_t GetAllocSize(const size_t requested);
	void* UpdateTrackerAllocation(void* ptr, const size_t size, const TrackerIndex tracker);
}

#endif