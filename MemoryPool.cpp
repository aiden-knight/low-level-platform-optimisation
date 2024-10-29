#include "MemoryPool.h"
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <cmath>

namespace MemoryPoolManager
{
	MemoryPool::MemoryPool(const size_t chunkSize, const size_t chunkNumber) :
		chunkSize{chunkSize},
		chunkCount{chunkNumber},
		byteCount{(size_t)std::ceil(chunkNumber / 4.0f)}
	{
		start = (Byte*)std::malloc(byteCount + (chunkSize * chunkNumber));
		if (start != nullptr)
			std::memset(start, 0, byteCount);
	}

	MemoryPool::~MemoryPool()
	{
		if (start != nullptr)
		{
			std::free(start);
			start = nullptr;
		}
	}

	void* MemoryPool::Allocate(size_t size)
	{
		// amount of chunks needed to store memory of size
		unsigned int chunksNeeded = (unsigned int) std::ceil((float)size / chunkSize);

		// loops through all toggle bits at start of pool to find enough contiguous space
		size_t bitsPos = 0;
		bool exit = false;
		for (unsigned int found = 0, bytePos = 0; !exit; ++bytePos)
		{
			Byte wholeByte = *(start + bytePos);

			for (unsigned int i = 0; i < 4; ++i)
			{
				// if first bit is not set
				if (!(wholeByte & occupiedMask))
				{
					// if found enough chunks
					if (++found == chunksNeeded)
					{
						bitsPos -= (chunksNeeded - 1); // start pos of needed chunks
						exit = true;
						break;
					}
				}
				else
				{
					found = 0; // reset count to 0
				}

				// shift to check next bit 2 over
				wholeByte <<= 2;
				if (++bitsPos == chunkCount) return nullptr; // if end of pool and not found enough space return nullptr
			}
		}

		// for loop sets relevant bits as occupied
		unsigned int bitOffset = (bitsPos % 4) * 2;
		Byte* byte = (start + (bitsPos / 4)); // first byte to edit
		for (unsigned int count = chunksNeeded; count != 0; --count)
		{
			// if at end then apply end mask to signify stop point for freeing
			if (count == 1)
				*byte |= occupiedMask >> bitOffset;
			else
				*byte |= combinedMask >> bitOffset;

			// increment bit offset by 2 as each check value takes up 2 bits
			// if bit offset is 8 then move to next byte and reset
			if ((bitOffset += 2) == 8)
			{
				byte++;
				bitOffset = 0;
			}
		}
		return start + byteCount + (bitsPos * chunkSize);
	}

	bool MemoryPool::Free(void* ptr)
	{
		const Byte* const poolStart = start + byteCount; // pointer to start of pool
		const unsigned int bitsPos = ((Byte*)ptr - poolStart) / chunkSize; // where the bits that need to be flipped begin

		// if the pointer is not within pool return failed deallocation
		if (bitsPos >= chunkCount) return false;

		// reset the bits to false to indicate empty chunk
		unsigned int bitOffset = (bitsPos % 4) * 2;
		Byte* byte = (start + (bitsPos / 4)); // first byte to edit
		bool continueLoop = true;
		unsigned int count = 0;
		while (continueLoop)
		{
			// check if should continue setting block as free
			continueLoop = (*byte & (continueMask >> bitOffset));

			// clear bits
			*byte &= ~(combinedMask >> bitOffset);
			count++;
			if ((bitOffset += 2) == 8)
			{
				byte++;
				bitOffset = 0;
			}
		}
		return true;
	}

	void MemoryPool::Print()
	{
		void* poolStart = start + byteCount;
		std::cout << "\nPrinting pool with chunk size: " << chunkSize << ", and chunk count of: " << chunkCount << std::endl;
		std::cout << "Pointer to start of first chunk: " << poolStart << std::endl;
		std::cout << "\nPool fill info - "
			<< "first bit is whether chunk is occupied, "
			<< "second is whether next chunk is used by same occupier as current chunk, "
			<< "chunks are separated by pipe character:\n|";
		size_t bitsPos = 0;
		for (unsigned int found = 0, bytePos = 0; ; ++bytePos)
		{
			Byte wholeByte = *(start + bytePos);

			for (unsigned int i = 0; i < 4; ++i)
			{
				bool occupied = (wholeByte & occupiedMask);
				bool continued = (wholeByte & continueMask);

				if (occupied)
					std::cout << "1";
				else
					std::cout << "0";

				if(continued)
					std::cout << "1";
				else
					std::cout << "0";
				std::cout << "|";

				// shift to check next bit 2 over
				wholeByte <<= 2;
				if (++bitsPos == chunkCount)
				{
					std::cout << std::endl;
					return;
				}
			}
		}
	}

	StaticMemoryPool::StaticMemoryPool(const size_t chunkSize, const size_t chunkCount) :
		chunkSize(chunkSize),
		chunkCount(chunkCount)
	{
		start = (Byte*)std::malloc(chunkSize * chunkCount);
		freeList = (void**)std::malloc(sizeof(void*) * chunkCount);
		freeChunkCount = 0;
		if (freeList != nullptr && start != nullptr)
		{
			for (size_t i = 0; i < chunkCount; ++i)
			{
				freeList[i] = start + (i * chunkSize);
			}
			freeChunkCount = chunkCount;
		}
	}

	StaticMemoryPool::~StaticMemoryPool()
	{
		std::free(start);
		std::free(freeList);
	}

	void* StaticMemoryPool::Allocate()
	{
		if(freeChunkCount == 0) return nullptr;

		return freeList[--freeChunkCount];
	}

	bool StaticMemoryPool::Free(void* ptr)
	{
		if (start <= ptr && ptr <= start + (chunkSize * chunkCount))
		{
			freeList[freeChunkCount++] = ptr;
			return true;
		}
		return false;
	}
}
