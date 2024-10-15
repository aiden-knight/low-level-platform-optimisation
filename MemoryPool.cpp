#include "MemoryPool.h"
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <cmath>
#include <bitset>

namespace MemoryPoolManager
{
	MemoryPool::MemoryPool(const size_t chunkSize, const size_t chunkNumber) :
		chunkSize{chunkSize},
		chunkNumber{chunkNumber},
		byteCount{(size_t)std::ceil(chunkNumber / 4.0f)}
	{
		start = (unsigned char*)std::malloc(byteCount + (chunkSize * chunkNumber));
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
			unsigned char wholeByte = *(start + bytePos);

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
				if (++bitsPos == chunkNumber) return nullptr; // if end of pool and not found enough space return nullptr
			}
		}

		// for loop sets relevant bits as occupied
		unsigned int bitOffset = (bitsPos % 4) * 2;
		unsigned char* byte = (start + (bitsPos / 4)); // first byte to edit
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
		const unsigned char* const poolStart = start + byteCount; // pointer to start of pool
		const unsigned int bitsPos = ((unsigned char*)ptr - poolStart) / chunkSize; // where the bits that need to be flipped begin

		// if the pointer is not within pool return failed deallocation
		if (bitsPos >= chunkNumber) return false;

		// reset the bits to false to indicate empty chunk
		unsigned int bitOffset = (bitsPos % 4) * 2;
		unsigned char* byte = (start + (bitsPos / 4)); // first byte to edit
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
		std::cout << "Dealloacted " << count << " blocks, max size possible is: " << chunkSize * count << std::endl;

		return true;
	}
	void MemoryPool::Output()
	{
		void* poolStart = start + byteCount;
		std::cout << "First pool chunk start: " << poolStart << std::endl;
		size_t bitsPos = 0;
		for (unsigned int found = 0, bytePos = 0; ; ++bytePos)
		{
			unsigned char wholeByte = *(start + bytePos);

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
				if (++bitsPos == chunkNumber)
				{
					std::cout << std::endl;
					return;
				}
			}
		}
	}
}
