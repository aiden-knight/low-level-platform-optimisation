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

	private:
		const size_t chunkSize;
		const size_t chunkNumber;
		const size_t byteCount;

		static constexpr char continueMask = 0b01000000;
		static constexpr char occupiedMask = 0b10000000;
		static constexpr char combinedMask = 0b11000000;

		unsigned char* start;

	};
}