#include "ThreadPool.h"
#include <thread>
#include <vector>
#include <queue>
#include <atomic>
#include <mutex>
#include <iostream>

namespace ThreadPool
{
	class Pool
	{
		std::vector<std::thread> threads;
		std::queue<std::function<void()>> tasks;
		std::mutex tasksMutex;
		std::condition_variable mutexCondition;
		bool shouldTerminate = false;

		std::atomic<size_t> taskCount = 0;
		std::mutex mainThreadMutex;
		std::condition_variable mainThreadCondition;

		void ThreadLoop()
		{
			while (true)
			{
				std::function<void()> task;

				{
					std::unique_lock<std::mutex> lock(tasksMutex);

					mutexCondition.wait(lock, [this]() {
						return !tasks.empty() || shouldTerminate;
						});

					if (shouldTerminate) return;

					task = tasks.front();
					tasks.pop();
				}
				task();
				--taskCount;
				mainThreadCondition.notify_one();
			}
		}

	public:
		Pool(size_t threadCount)
		{
			threads = std::vector<std::thread>(threadCount);
			for (int i = 0; i < threadCount; i++)
			{
				threads[i] = std::thread(&Pool::ThreadLoop, this);
			}
		}

		~Pool()
		{
			{
				std::unique_lock<std::mutex> lock(tasksMutex);
				shouldTerminate = true;
			}
			mutexCondition.notify_all();

			for (std::thread& thread : threads)
			{
				thread.join();
			}
			threads.clear();
		}

		void PushTask(const std::function<void()>& task)
		{
			{
				std::unique_lock<std::mutex> lock(tasksMutex);
				tasks.push(task);
				++taskCount;
			}
			mutexCondition.notify_one();
		}

		bool Busy()
		{
			bool poolBusy;
			{
				std::unique_lock<std::mutex> lock(tasksMutex);
				poolBusy = !tasks.empty();
			}
			return poolBusy;
		}

		void WaitForCompletion()
		{
			while (taskCount != 0)
			{
				
			}
		}
	};

	namespace
	{
		Pool* pool = nullptr;
	}


	void Init(size_t threadCount)
	{
		if (pool == nullptr)
		{
			pool = new Pool(threadCount);
		}
	}

	void PushTask(const std::function<void()>& task)
	{
		pool->PushTask(task);
	}

	bool Busy()
	{
		return pool->Busy();
	}

	void WaitForCompletion()
	{
		pool->WaitForCompletion();
	}

	void Destroy()
	{
		delete pool;
		pool = nullptr;
	}
}