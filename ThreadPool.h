#pragma once
#include <thread>
#include <vector>
#include <queue>
#include <atomic>
#include <mutex>
#include <iostream>
#include <functional>


class ThreadPool
{
	// vector of threads created at construction
	std::vector<std::thread> threads;

	// queue of functions to run on threads
	std::queue<std::function<void()>> tasks;
		
	// mutex to lock access to tasks queue
	std::mutex tasksMutex;

	// condition variable to signal when threads should check for new task or terminate
	std::condition_variable taskCondition;
	bool shouldTerminate = false;

	// variables used to wait for all tasks to be executed
	unsigned int taskCount = 0;
	std::condition_variable allTasksFinished;

	void ThreadLoop();
	ThreadPool(size_t threadCount);
	~ThreadPool();

	static ThreadPool* instance;

public:
	static ThreadPool* GetInstance();
	static ThreadPool* GetInstance(size_t threadCount);
	void Destruct();


	bool Busy();
	void PushTask(const std::function<void()>& task);
	void WaitForCompletion();
};