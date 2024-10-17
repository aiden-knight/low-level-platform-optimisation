#ifdef _DEBUG
#include <iostream>
#include <exception>
#include "MemoryManager.h"
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

	namespace // VARIABLES
	{
		/// <summary>
		/// Heap allocated array for trackers
		/// </summary>
		Tracker* trackers = nullptr;

#define TI(name) #name
		const char* TrackerNames[] = { TRACKERS };
#undef TI

		Header* startHeader = nullptr; // where to begin walking the heap from
		Header* endHeader = nullptr; // to add new headers to the end
	} // END VARIABLES

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
		if (trackers == nullptr) return;

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

	inline void HeapWalkFinished(bool foundCorruptFooter, unsigned int blocksChecked, bool walkedFull = true)
	{
		std::cout << (walkedFull ? "Walked full heap" : "May have missed some values due to early exit!") << std::endl;
		std::cout << "Found corrupt footer = " << foundCorruptFooter << std::endl;
		std::cout << "Blocks checked = " << blocksChecked << std::endl;
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
		unsigned int blocksChecked = 0;

		// forward traverse
		while (header != nullptr)
		{
			std::cout << "Block " << (++blocksChecked) << ": ";

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
			HeapWalkFinished(foundCorruptFooter, blocksChecked);	
			return; 
		}

		// traverse backwards until hits corrupt header
		Header* headerReverse = endHeader;
		std::cout << "\nTraversing backwards" << std::endl;
		while (headerReverse != header)
		{
			std::cout << "Block " << (++blocksChecked) << ": ";
			if (headerReverse->checkVal == headerCheckValue)
			{
				if (PrintHeaderInfo(headerReverse)) foundCorruptFooter = true;
			}
			else
			{
				std::cout << "Header check value corrupt whilst backward traversing, header ptr:" << headerReverse << std::endl;
				HeapWalkFinished(foundCorruptFooter, blocksChecked, false);
				return;
			}

			// go to previous header
			headerReverse = headerReverse->prev;
		}

		// means we reached the end
		if (headerReverse == header)
		{
			HeapWalkFinished(foundCorruptFooter, blocksChecked);
			return;
		}
	}

	void* UpdateTrackerDeallocation(void* ptr)
	{
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

		return ptr;
	}

	const size_t GetAllocSize(const size_t requested)
	{
		return sizeof(Header) + requested + sizeof(Footer);
	}

	void* UpdateTrackerAllocation(void* ptr, const size_t size, const TrackerIndex tracker)
	{
		static char initialised = InitTrackers(); // use of static variable initialisation to only init the trackers once

		Header* headerPtr = (Header*)ptr;

		// placement new construct header
		new (headerPtr) Header(size, tracker);
		(trackers + tracker)->Allocation(size); // add memory to relevant tracker

		// placement new constuct footer
		new ((char*)(headerPtr + 1) + size) Footer();

		// if startHeader hasn't been stored already, store created header there
		if (startHeader == nullptr)
		{
			startHeader = headerPtr;
		}
		else // make old end point to current header as next and current point to old as previous
		{
			endHeader->next = headerPtr;
			headerPtr->prev = endHeader;
		}
		endHeader = headerPtr; // update the new end header

		// return pointer to requested memory
		return headerPtr + 1;
	}

	void Cleanup()
	{
		if (trackers != nullptr)
		{
			OutputAllocations();

			std::free(trackers);
			trackers = nullptr;
		}

	}
}

#endif