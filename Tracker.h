#pragma once
#include <iostream>

enum TrackerIndex
{
	DEFAULT,
	BOX,
	SPHERE,
	ARRAY_SIZE
};

class Tracker
{
public:
	Tracker(TrackerIndex index)
	{
		this->index = index;
#ifdef _DEBUG

#else
		allocatedMemory = 0;
#endif // !_DEBUG
	}

	inline void Allocation(const size_t amount) 
	{ 
		allocatedMemory += amount;
		if (index == DEFAULT) {
			std::cout << "Default Alloc: " << amount << std::endl;
		}
	}

	inline void Deallocation(const size_t amount) 
	{ 
		allocatedMemory -= amount;
		if (index == DEFAULT) {
			std::cout << "Default Dealloc: " << amount << std::endl;
		}
	}

	inline size_t GetAllocated() const { return allocatedMemory; }
private:
	size_t allocatedMemory;
	TrackerIndex index;
};