#ifdef _DEBUG

#include "MemoryManager.h"
#include <iostream>
#include <cstdlib>
#include <exception>
#include "Tracker.h"

namespace MemoryManager
{
	constexpr uint32_t headerCheckValue = 0x01ABCDEF;
	constexpr uint32_t footerCheckValue = 0xFEDCBA10;

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

		Header(size_t size, TrackerIndex tracker) 
		{
			allocationSize = size; 
			trackerIndex = tracker;
			checkVal = headerCheckValue;
			next = nullptr;
			prev = nullptr;
		}
	};

	struct Footer
	{
		uint32_t checkVal;

		Footer() { checkVal = footerCheckValue; }
	};

	// VARIABLES
	namespace
	{
		/// <summary>
		/// Heap allocated array for trackers
		/// </summary>
		Tracker* trackers;

#define TI(name) #name
		const char* TrackerNames[] = { TRACKERS };
#undef TI

		Header* startHeader = nullptr; // where to begin walking the heap from
		Header* endHeader = nullptr; // to add new headers to the end
	}

	char InitTrackers()
	{
		trackers = (Tracker*)std::malloc(sizeof(Tracker) * NUM_TRACKERS);
		int i = 0;
		for (int i = 0; i < NUM_TRACKERS; i++)
		{
			void* dst = trackers + i;
			new (dst) Tracker((TrackerIndex)i);
		}

		return 0;
	}

	void OutputAllocations()
	{
		std::cout << "\nMemory Trackers:\n";
		for (int i = 0; i < NUM_TRACKERS; i++)
		{
			std::cout << TrackerNames[i] << " - " << trackers[i].GetAllocated() << " Bytes\n";
		}
	}

	inline bool PrintHeaderInfo(Header* header)
	{
		Footer* footer = (Footer*)((char*)header + sizeof(Header) + header->allocationSize);
		bool footerCorrupt = footer->checkVal != footerCheckValue;

		std::cout 
			<< header + 1 
			<< ": tracker[" << TrackerNames[header->trackerIndex] 
			<< "] blocksize[" << header->allocationSize 
			<< "] footer overwritten = " << footerCorrupt 
		<< '\n';
		return footerCorrupt;
	}

	void WalkHeap()
	{
		if (startHeader == nullptr)
		{
			std::cout << "No memory allocations detected" << std::endl;
			return;
		}

		Header* header = startHeader;
		std::cout << std::boolalpha;
		bool foundCorruptFooter = false;

		// forward traverse
		while (header != nullptr)
		{
			if (header->checkVal == headerCheckValue)
			{
				if (PrintHeaderInfo(header)) foundCorruptFooter = true;
			}
			else
			{
				std::cout << "Header check value corrupt whilst forward traversing, header ptr:" << header << std::endl;
				if (header == endHeader) header = nullptr; // as we saw all headers
				break;
			}

			// go to next header
			header = header->next;
		}

		// means we reached the end
		if (header == nullptr)
		{
			std::cout << "Walked full heap" << std::endl;
			std::cout << "Found corrupt footer = " << foundCorruptFooter << std::endl;
			return;
		}

		// traverse backwards until hits corrupt header
		Header* headerReverse = endHeader;
		std::cout << "\nTraversing backwards" << std::endl;
		while (headerReverse != header)
		{
			if (headerReverse->checkVal == headerCheckValue)
			{
				if (PrintHeaderInfo(headerReverse)) foundCorruptFooter = true;
			}
			else
			{
				std::cout << "Header check value corrupt whilst backward traversing, header ptr:" << headerReverse << std::endl;
				std::cout << "May have missed some values due to early exit!" << std::endl;
				std::cout << "Found corrupt footer = " << foundCorruptFooter << std::endl;
				return;
			}

			// go to previous header
			headerReverse = headerReverse->prev;
		}

		// means we reached the end
		if (headerReverse == header)
		{
			std::cout << "Walked full heap" << std::endl;
			std::cout << "Found corrupt footer = " << foundCorruptFooter << std::endl;
			return;
		}
	}
}

using namespace MemoryManager;

/// <summary>
/// Global new for default memory allocation
/// </summary>
void* operator new(size_t size)
{
	return ::operator new(size, TrackerIndex::Default); // calls global new with Default tracker index passed through
}

/// <summary>
/// Global delete overloaded to ensure header and footer data is cleaned up
/// </summary>
/// <param name="ptr"></param>
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

	trackers[header->trackerIndex].Deallocation(header->allocationSize);

	// update start and end header pointers if needed
	if (header == startHeader) startHeader = header->next;
	if (header == endHeader) endHeader = header->prev;

	// update previous and next headers where applicable
	if (header->next != nullptr) header->next->prev = header->prev;
	if (header->prev != nullptr) header->prev->next = header->next;
	
	std::free(ptr);
}

void* operator new(size_t size, TrackerIndex tracker)
{
	static char initialised = InitTrackers(); // use of static variable initialisation to only init the trackers once

	// allocate memory
	size_t allocSize = sizeof(Header) + size + sizeof(Footer);
	Header* ptr = (Header*)std::malloc(allocSize);

	// placement new construct header
	new (ptr) Header(size, tracker);
	(trackers + tracker)->Allocation(size); // add memory to relevant tracker

	// placement new constuct footer
	new ((char*)(ptr + 1) + size) Footer();

	// if startHeader hasn't been stored already, store created header there
	if (startHeader == nullptr)
	{
		startHeader = ptr;
	}
	else // make old end point to current header as next and current point to old as previous
	{
		endHeader->next = ptr;
		ptr->prev = endHeader;
	}
	endHeader = ptr; // update the new end header

	// return pointer to requested memory
	return ptr + 1;
}

#endif