#ifdef _DEBUG

#include "MemoryManager.h"
#include <iostream>
#include <cstdlib>
#include <string>
#include "ColliderObject.h"

#include <exception>

namespace MemoryManager
{
	namespace
	{
		/// <summary>
		/// Heap allocated array for trackers
		/// </summary>
		Tracker* trackers;

#define TI(name) #name
		const char* TrackerNames[] = { TRACKERS };
#undef TI
	}

	constexpr uint32_t headerCheckValue = 0x01ABCDEF;
	constexpr uint32_t footerCheckValue = 0xFEDCBA10;

	/// <summary>
	/// Header to track allocation size and relevant memory tracker 
	/// </summary>
	struct Header
	{
		size_t allocationSize;
		Tracker* trackerPtr;
		uint32_t checkVal;

		Header(size_t size, Tracker* ptr) { allocationSize = size; trackerPtr = ptr; checkVal = headerCheckValue; }
	};


	struct Footer
	{
		uint32_t checkVal;

		Footer() { checkVal = footerCheckValue; }
	};

	char InitTrackers()
	{
		trackers = (Tracker*)std::malloc(sizeof(Tracker) * NUM_TRACKERS);
		int i = 0;
		for (int i = 0; i < NUM_TRACKERS; i++)
		{
			void* dst = trackers + i;
			new (dst) Tracker((TrackerIndex)i);
		}

		std::cout << "Initialised Trackers" << std::endl;
		return 0;
	}

	void OutputAllocations()
	{
		for (int i = 0; i < NUM_TRACKERS; i++)
		{
			std::cout << TrackerNames[i] << ": " << trackers[i].GetAllocated() << std::endl;
		}
	}
}

using namespace MemoryManager;

void* operator new(size_t size)
{
	static char initialised = InitTrackers();

	return ::operator new(size, TrackerIndex::Default);
}

void operator delete(void* ptr)
{
	if (ptr == nullptr) return;

	ptr = (char*)ptr - sizeof(Header);
	
	Header* header = (Header*)ptr;
	Footer* footer = (Footer*)((char*)ptr + sizeof(Header) + header->allocationSize);

	if (header->checkVal != headerCheckValue || footer->checkVal != footerCheckValue)
	{
		throw std::overflow_error("Check values incorrect");
	}

	if (header->trackerPtr) header->trackerPtr->Deallocation(header->allocationSize);

	std::free(ptr);
}

void* operator new(size_t size, TrackerIndex tracker)
{
	// allocate memory
	size_t allocSize = sizeof(Header) + size + sizeof(Footer);
	void* ptr = std::malloc(allocSize);

	// placement new construct header
	new (ptr) Header(size, trackers + tracker);
	(trackers + tracker)->Allocation(size); // add memory to relevant tracker

	// placement new constuct footer
	new ((char*)ptr + sizeof(Header) + size) Footer();

	// return pointer to requested memory
	return (char*)ptr + sizeof(Header);
}

#endif