#pragma once

namespace MemoryPoolManager
{
	void* RequestMemory(size_t size);
	bool FreeMemory(void* ptr);
}