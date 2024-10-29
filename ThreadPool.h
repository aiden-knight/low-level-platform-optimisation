#pragma once
#include <functional>

namespace ThreadPool
{
	void Init(size_t threadCount);
	void PushTask(const std::function<void()>& task);
	bool Busy();
	void WaitForCompletion();
	void Destroy();
}