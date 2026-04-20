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

	template<typename TerminateCondition, typename Func, typename... Args>
	inline bool InvokeWithTimeBudget(TerminateCondition&& terminateCondition, std::chrono::nanoseconds timeBudget, Func&& func, Args&&... args)
	{
		auto startTime = std::chrono::high_resolution_clock::now();
		while (!terminateCondition())
		{
			func(args...);
			auto currentTime = std::chrono::high_resolution_clock::now();
			if (currentTime - startTime >= timeBudget) { return false; }
		}
		return true;
	}
}
