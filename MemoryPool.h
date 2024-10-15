#pragma once

namespace MemoryPoolManager
{
	template<size_t chunkNumber>
	class MemoryPool
	{
	private:
		const size_t chunkSize;
		constexpr size_t byteCount = (chunkNumber/4) + 1

		char* start;

	public:
		MemoryPool(const size_t chunkSize) :
			chunkSize{ chunkSize }
		{
			start = (char*)std::malloc(byteCount + (chunkSize * chunkNumber));
			if (start != nullptr)
				std::memset(start, 0, byteCount);
		}

		~MemoryPool()
		{
			if (start != nullptr)
				std::free(start);
		}

		void* Allocate(size_t size)
		{
			// amount of chunks needed to store memory of size
			unsigned int chunksNeeded = (size / chunkSize) + 1;

			size_t bitsPos = 0;
			for (unsigned int found = 0, bytePos = 0; bitPos < chunkNumber; ++bytePos)
			{
				char wholeByte = *(start + bytePos);

				for (unsigned int i = 0; i < 4; ++i)
				{
					if (!(wholeByte & 0x10000000) && ++found == chunksNeeded)
					{

					}
					else
					{
						found = 0;
					}

					wholeByte <<= 2;
					if (++bitPos == chunkNumber) break;
				}
			}

			if (bitsPos == chunksNeeded) return nullptr;

			for (unsigned int i = 0; i < chunksNeeded; ++i)
			{
				bits.set(bitsPos + i);
			}
			return start + byteCount + (bitsPos * chunkSize);
		}

		bool Free(void* ptr)
		{
			// amount of chunks being freed
			unsigned int freedAmount = (size / chunkSize) + 1;

			char* poolStart = start + chunkNumber; // pointer to start of pool
			unsigned int byteOffset = ((char*)ptr - poolStart) / chunkSize; // where the bits that need to be flipped are

			// if the pointer is not within pool return failed deallocation
			if (byteOffset >= byteCount) return false;

			// reset the bits to false
			std::bitset<chunkNumber> bits = std::bitset<chunkNumber>(start, chunkNumber);
			for (size_t bit = byteOffset; bit < byteOffset + freedAmount; ++bit)
			{
				bits.reset(bit);
			}

			return true;
		}
	};
}