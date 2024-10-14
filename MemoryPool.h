#pragma once
#include <bitset>

namespace MemoryPoolManager
{
	class MemoryPool
	{
	public:
		MemoryPool(const size_t chunkSize, const unsigned int chunkNumber);
		~MemoryPool();

		void* Allocate(size_t size);
		void Free(void* ptr, size_t size);
	private:
		const size_t chunkSize;
		const unsigned int chunkNumber;
		const unsigned int byteCount;

		char* start;
	};
}