#pragma once

namespace MemoryPoolManager
{
	void* RequestMemory(size_t size);

	/// <summary>
	/// Given a pointer to some memory, sets the block as free in relevant pool
	/// </summary>
	/// <returns>false if memory is not contained in any pools</returns>
	bool FreeMemory(void* ptr);

	void PrintPoolDebugInfo();
}