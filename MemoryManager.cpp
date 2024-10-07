#ifdef _DEBUG

#include "MemoryManager.h"
#include <iostream>
#include <cstdlib>
#include <array>
#include <string>
#include "ColliderObject.h"

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
		Tracker* trackers;
	}

	char InitTrackers()
	{
		trackers = (Tracker*)std::malloc(sizeof(Tracker) * ARRAY_SIZE);
		int i = 0;
		for (int i = 0; i < ARRAY_SIZE; i++)
		{
			void* dst = trackers + i;
			new (dst) Tracker((TrackerIndex)i);
		}

		std::cout << "Initialised Trackers" << std::endl;
		return 0;
	}

	void OutputAllocations()
	{
		std::cout << "Default: " << trackers[DEFAULT].GetAllocated() << std::endl;
		std::cout << "Box: " << trackers[BOX].GetAllocated() << std::endl;
		std::cout << "Sphere: " << trackers[SPHERE].GetAllocated() << '\n' << std::endl;
	}
}

using namespace MemoryManager;

void* operator new(size_t size)
{
	static char initialised = InitTrackers();

	return ::operator new(size, DEFAULT);
}

void operator delete(void* ptr)
{
	if (ptr == nullptr) return;

	ptr = (char*)ptr - sizeof(Header);
	
	Header* header = (Header*)ptr;
	if (header->trackerPtr) header->trackerPtr->Deallocation(header->allocationSize);

	std::free(ptr);
}

void* operator new(size_t size, TrackerIndex tracker)
{
	size_t allocSize = sizeof(Header) + size;

	void* ptr = std::malloc(allocSize);
	new (ptr) Header(size, trackers + tracker);
	(trackers + tracker)->Allocation(size);

	return (char*)ptr + sizeof(Header);
}

#endif