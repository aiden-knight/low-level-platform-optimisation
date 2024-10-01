#include "MemoryManager.h"
#include <iostream>
#include <cstdlib>
#include <array>
#include <string>

namespace MemoryManager
{
	struct Header
	{
		size_t allocationSize;
		Tracker* trackerPtr;

		Header(size_t size, Tracker* ptr) { allocationSize = size; trackerPtr = ptr; }
	};

	namespace
	{
		std::array<Tracker, ARRAY_SIZE> trackers = { Tracker(DEFAULT), Tracker(BOX), Tracker(SPHERE) };
	}

	void OutputAllocations()
	{
		std::cout << "Default: " << trackers[DEFAULT].GetAllocated() << std::endl;
		std::cout << "Box: " << trackers[BOX].GetAllocated() << std::endl;
		std::cout << "Sphere: " << trackers[SPHERE].GetAllocated() << std::endl;
	}
}

using namespace MemoryManager;

void* operator new(size_t size)
{
	return ::operator new(size, DEFAULT);
}

void operator delete(void* ptr)
{
	ptr = (char*)ptr - sizeof(Header);
	
	Header* header = (Header*)ptr;
	if (header->trackerPtr) header->trackerPtr->Deallocation(header->allocationSize);

	std::free(ptr);
}

void* operator new(size_t size, TrackerIndex tracker)
{
	size_t allocSize = sizeof(Header) + size;

	void* ptr = std::malloc(allocSize);
	new (ptr) Header(size, &trackers[tracker]);
	trackers[tracker].Allocation(size);

	return (char*)ptr + sizeof(Header);
}
