#pragma once
#include <chrono>

/// <summary>
/// 
/// </summary>
/// <typeparam name="ClockType">i.e. std::chrono::steady_clock</typeparam>
/// <typeparam name="Period">i.e. std::milli</typeparam>
template <class ClockType, class Period>
class Timer
{
	using TimePoint = typename ClockType::time_point;
public:
	Timer()
	{
		start = ClockType::now();
	}

	float Elapsed() const
	{
		std::chrono::duration<float, Period> duration = (ClockType::now() - start);
		return duration.count();
	}

private:
	TimePoint start;
};