#include "ThreadPool.h"


ThreadPool* ThreadPool::instance = nullptr;

void ThreadPool::ThreadLoop()
{
	while (!shouldTerminate)
	{
		std::unique_lock<std::mutex> lock(tasksMutex);

		taskCondition.wait(lock, [this]() {
			return !tasks.empty() || shouldTerminate;
			});
		
		if (!tasks.empty())
		{
			++taskCount;
			std::function<void()> task = tasks.front();
			tasks.pop();

			lock.unlock();
			task();
			lock.lock();

			--taskCount;
			allTasksFinished.notify_one();
		}
	}
}

ThreadPool::ThreadPool(size_t threadCount)
{
	threads = std::vector<std::thread>(threadCount);
	for (int i = 0; i < threadCount; i++)
	{
		threads[i] = std::thread(&ThreadPool::ThreadLoop, this);
	}

}

ThreadPool::~ThreadPool()
{
	{
		std::unique_lock<std::mutex> lock(tasksMutex);
		shouldTerminate = true;
	}
	taskCondition.notify_all();

	for (std::thread& thread : threads)
	{
		thread.join();
	}
	threads.clear();
}

void ThreadPool::PushTask(const std::function<void()>& task)
{
	{
		std::unique_lock<std::mutex> lock(tasksMutex);
		tasks.push(task);
	}
	taskCondition.notify_one();
}

ThreadPool* ThreadPool::GetInstance()
{
	return instance;
}

ThreadPool* ThreadPool::GetInstance(size_t threadCount)
{
	if (instance == nullptr)
		instance = new ThreadPool(threadCount);
		
	return instance;
}

void ThreadPool::Destruct()
{
	delete this;
	instance = nullptr;
}

bool ThreadPool::Busy()
{
	bool poolBusy;
	{
		std::unique_lock<std::mutex> lock(tasksMutex);
		poolBusy = !tasks.empty();
	}
	return poolBusy;
}

void ThreadPool::WaitForCompletion()
{
	std::unique_lock<std::mutex> lock(tasksMutex);
	allTasksFinished.wait(lock, [this]() { return tasks.empty() && (taskCount == 0); });
}