#pragma once

#include<string>
#include<chrono>

namespace Pitaya::Core
{
	inline std::string Date()
	{
		auto t = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
		struct tm buf;
		localtime_s(&buf, &t);
		char temp[32] = {};
		std::strftime(temp, sizeof(temp), "%Y-%m-%d %H:%M:%S", &buf);
		return temp;
	}
	inline std::string Data(std::chrono::system_clock::time_point tp)
	{
		auto t = std::chrono::system_clock::to_time_t(tp);
		struct tm buf;
		localtime_s(&buf, &t);
		char temp[32] = {};
		std::strftime(temp, sizeof(temp), "%Y-%m-%d %H:%M:%S", &buf);
		return temp;
	}
	inline std::string Data(time_t tp)
	{
		struct tm buf;
		localtime_s(&buf, &tp);
		char temp[32] = {};
		std::strftime(temp, sizeof(temp), "%Y-%m-%d %H:%M:%S", &buf);
		return temp;
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
