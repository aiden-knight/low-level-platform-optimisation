#include "ThreadPool.h"
#include <thread>
#include <vector>
#include <queue>
#include <functional>

namespace ThreadPool
{
	namespace
	{
		std::vector<std::thread> threads;
		std::queue<std::function<void()>> tasks;
	}

	void Init(size_t threadCount)
	{
		threads = std::vector<std::thread>(threadCount);
		
	}
}