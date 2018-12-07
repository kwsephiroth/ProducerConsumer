#pragma once
//#include <iostream>
#include <chrono>

using TimePoint = std::chrono::high_resolution_clock::time_point;
using HiResClock = std::chrono::high_resolution_clock;

enum class TimeResolution
{
	Hours = 6,
	Minutes = 5,
	Seconds = 4,
	Milliseconds = 3,
	Microseconds = 2,
	Nanoseconds = 1
};

std::string TimeResolutionToString(TimeResolution resolution)
{
	switch (resolution)
	{
		case TimeResolution::Hours:
			return "hours";
		case TimeResolution::Minutes:
			return "minutes";
		case TimeResolution::Seconds:
			return "seconds";
		case TimeResolution::Milliseconds:
			return "milliseconds";
		case TimeResolution::Microseconds:
			return "microseconds";
		case TimeResolution::Nanoseconds:
			return "nanoseconds";
		default:
			return "";
	}
}

class ExecutionTimer
{
private:
public:
	ExecutionTimer() = default;
	~ExecutionTimer() = default;

	template<typename Func, typename... Args>
	static long double GetExecutionTime(TimeResolution resolution, Func func, Args&&... args);
	static long double ExecutionTimer::GetExecutionTime(const TimePoint& start, const TimePoint& finish, TimeResolution resolution);
};

template<typename Func, typename... Args>
long double ExecutionTimer::GetExecutionTime(TimeResolution resolution, Func func, Args&&... args)
{
	TimePoint start = HiResClock::now();
	func(std::forward<Args>(args)...);
	TimePoint finish = HiResClock::now();

	auto elapsed = finish - start;

	switch (resolution)
	{
	case TimeResolution::Hours:
		return std::chrono::duration_cast<std::chrono::hours>(elapsed).count();
	case TimeResolution::Minutes:
		return std::chrono::duration_cast<std::chrono::minutes>(elapsed).count();
	case TimeResolution::Seconds:
		//return std::chrono::duration_cast<std::chrono::seconds>(elapsed).count();
		return std::chrono::duration<long double>(elapsed).count();
	case TimeResolution::Milliseconds:
		return std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count();
	case TimeResolution::Microseconds:
		return std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
	case TimeResolution::Nanoseconds:
		return std::chrono::duration_cast<std::chrono::nanoseconds>(elapsed).count();
	default:
		return -1;
	}
}


long double ExecutionTimer::GetExecutionTime(const TimePoint& start, const TimePoint& finish, TimeResolution resolution)
{
	auto elapsed = finish - start;

	switch (resolution)
	{
	case TimeResolution::Hours:
		return std::chrono::duration_cast<std::chrono::hours>(elapsed).count();
	case TimeResolution::Minutes:
		return std::chrono::duration_cast<std::chrono::minutes>(elapsed).count();
	case TimeResolution::Seconds:
		//return std::chrono::duration_cast<std::chrono::seconds>(elapsed).count();
		return std::chrono::duration<long double>(elapsed).count();
	case TimeResolution::Milliseconds:
		return std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count();
	case TimeResolution::Microseconds:
		return std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
	case TimeResolution::Nanoseconds:
		return std::chrono::duration_cast<std::chrono::nanoseconds>(elapsed).count();
	default:
		return -1;
	}
}