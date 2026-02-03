#pragma once

#include"../Define/Define.h"
#include"../Enum/Enum.h"

#include<fstream>
#include<sstream>
#include<string>
#include<chrono>
#include<iomanip>
#include<mutex>
#include<condition_variable>
#include<queue>
#include<thread>
#include<atomic>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

namespace Pitaya::Core::Log
{
	class LogManager
	{
		DECLARE_SINGLETON_CLASS_RB(LogManager)

	private:
		struct LogMessage
		{
			LogLevel level;
			std::string time;
			std::string message;
			std::string thread;
		};

	public:
		void Log(LogLevel level, const std::string& message);

	private:
		void LogThread();
		inline std::string LogLevelToString(Core::Log::LogLevel level)
		{
			switch (level)
			{
				case Core::Log::LogLevel::Info:		return "Info";
				case Core::Log::LogLevel::Debug:	return "Debug";
				case Core::Log::LogLevel::Warning:	return "Warning";
				case Core::Log::LogLevel::Error:	return "Error";
				default:							return "";
			}
		}

	private:
		std::ofstream ofs;
		std::mutex mutex;
		std::queue<LogMessage> queue;
		std::condition_variable cond;
		std::thread logThread;
		std::atomic<bool> isRunning = true;
		static inline constexpr const char* fileName = "log.txt";
	};
}