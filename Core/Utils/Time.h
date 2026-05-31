#pragma once

#include<chrono>

namespace Pitaya::Core
{
	template<size_t N>
	inline void Date(char(&temp)[N])
	{
		auto t = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
		struct tm buf;
		localtime_s(&buf, &t);
		std::strftime(temp, N, "%Y-%m-%d %H:%M:%S", &buf);
	}
	template<size_t N>
	inline void Date(char(&temp)[N], std::chrono::system_clock::time_point tp)
	{
		auto t = std::chrono::system_clock::to_time_t(tp);
		struct tm buf;
		localtime_s(&buf, &t);
		std::strftime(temp, N, "%Y-%m-%d %H:%M:%S", &buf);
	}
	template<size_t N>
	inline void Date(char(&temp)[N], time_t tp)
	{
		struct tm buf;
		localtime_s(&buf, &tp);
		std::strftime(temp, N, "%Y-%m-%d %H:%M:%S", &buf);
	}
	inline void Date(char* temp, size_t size)
	{
		auto t = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
		struct tm buf;
		localtime_s(&buf, &t);
		std::strftime(temp, size, "%Y-%m-%d %H:%M:%S", &buf);
	}
	inline void Date(char* temp, size_t size, std::chrono::system_clock::time_point tp)
	{
		auto t = std::chrono::system_clock::to_time_t(tp);
		struct tm buf;
		localtime_s(&buf, &t);
		std::strftime(temp, size, "%Y-%m-%d %H:%M:%S", &buf);
	}
	inline void Date(char* temp, size_t size, time_t tp)
	{
		struct tm buf;
		localtime_s(&buf, &tp);
		std::strftime(temp, size, "%Y-%m-%d %H:%M:%S", &buf);
	}

	template<typename TerminateCondition, typename Func>
	inline bool InvokeWithTimeBudget(TerminateCondition&& terminateCondition, Func&& func, std::chrono::nanoseconds timeBudget)
	{
		if (terminateCondition()) { return true; }
		const auto startTime = std::chrono::steady_clock::now();
		do { func(); if (terminateCondition()) { return true; } }
		while (std::chrono::steady_clock::now() - startTime < timeBudget);
		return false;
	}
}
