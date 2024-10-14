#include "MemoryPool.h"
#include <cstdlib>

namespace MemoryPoolManager
{
	MemoryPool::MemoryPool(const size_t chunkSize, const unsigned int chunkNumber) :
		chunkSize{ chunkSize },
		chunkNumber{ ((chunkNumber / 8) + 1) * 8 },
		byteCount{ (chunkNumber / 8) + 1 }
	{
		start = (char*)std::malloc(byteCount + (chunkSize * chunkNumber));
		if(start != nullptr)
			std::memset(start, 0, byteCount);
	}

	MemoryPool::~MemoryPool()
	{
		if(start != nullptr)
			std::free(start);
	}

	void* MemoryPool::Allocate(size_t size)
	{
		// amount of chunks needed to store memory of size
		unsigned int chunksNeeded = (size / chunkSize) + 1;

		for (unsigned int i = 0; i < chunkNumber; ++i)
		{
			
		}
	}

	void MemoryPool::Free(void* ptr, size_t size)
	{
		// amount of chunks being freed
		unsigned int freedAmount = (size / chunkSize) + 1;

		char* poolStart = start + byteCount; // pointer to start of pool
		unsigned int byteOffset = ((char*)ptr - poolStart) / chunkSize; // where the bits that need to be flipped are 

		// reset the bits to false
		std::memset(start + byteOffset, 0, freedAmount);
	}
}