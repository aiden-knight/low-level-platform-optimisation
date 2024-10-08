#pragma once
#ifdef _DEBUG
#include <iostream>

#define TRACKERS \
TI(Default), \
TI(Box), \
TI(Sphere)


namespace MemoryManager
{
#define TI(name) name
	enum TrackerIndex
	{
		TRACKERS,
		NUM_TRACKERS
	};
#undef TI

	class Tracker
	{
	public:
		Tracker(TrackerIndex index)
		{
			this->index = index;
			allocatedMemory = 0;
		}

		inline void Allocation(const size_t amount)
		{
			allocatedMemory += amount;
			if (index == TrackerIndex::Default) {
				std::cout << "Default Alloc: " << amount << std::endl;
			}
		}

		inline void Deallocation(const size_t amount)
		{
			allocatedMemory -= amount;
			if (index == TrackerIndex::Default) {
				std::cout << "Default Dealloc: " << amount << std::endl;
			}
		}

		inline size_t GetAllocated() const { return allocatedMemory; }
	private:
		size_t allocatedMemory;
		TrackerIndex index;
	};
}

#endif