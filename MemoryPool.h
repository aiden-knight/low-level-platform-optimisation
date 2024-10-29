#pragma once
#include <cstdlib>

namespace MemoryPoolManager
{
	class MemoryPool
	{
		using Byte = unsigned char;

	public:
		MemoryPool(const size_t chunkSize, const size_t chunkCount);

		~MemoryPool();

		void* Allocate(size_t size);

		bool Free(void* ptr);

		void Print();

	private:
		const size_t chunkSize;
		const size_t chunkCount;
		const size_t byteCount;

		static constexpr Byte continueMask = 0b01000000;
		static constexpr Byte occupiedMask = 0b10000000;
		static constexpr Byte combinedMask = 0b11000000;

		Byte* start;
	};

	class StaticMemoryPool
	{

		using Byte = unsigned char;

	public:
		StaticMemoryPool(const size_t chunkSize, const size_t chunkCount);
		~StaticMemoryPool();

		void* Allocate();
		bool Free(void* ptr);

	private:
		const size_t chunkSize;
		const size_t chunkCount;

		Byte* start;
		void** freeList;
		unsigned int freeChunkCount;
	};
}