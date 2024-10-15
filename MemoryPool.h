#pragma once
#include <cstdlib>

namespace MemoryPoolManager
{
	class MemoryPool
	{
	public:
		MemoryPool(const size_t chunkSize, const size_t chunkNumber);

		~MemoryPool();

		void* Allocate(size_t size);

		bool Free(void* ptr);

		void Output();

	private:
		const size_t chunkSize;
		const size_t chunkNumber;
		const size_t byteCount;

		static constexpr unsigned char continueMask = 0b01000000;
		static constexpr unsigned char occupiedMask = 0b10000000;
		static constexpr unsigned char combinedMask = 0b11000000;

		unsigned char* start;

	};
}