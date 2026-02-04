#pragma once

#include"../Singleton/Singleton.h"
#include"../Enum/Enum.h"
#include"../Utility/Utility.h"

#include<fstream>
#include<sstream>
#include<string>
#include<chrono>
#include<iomanip>
#include<mutex>
#include<condition_variable>
#include<queue>
#include<atomic>
#include<filesystem>
#include<thread>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

namespace Pitaya::Core::Log
{
	class LogManager : public Pitaya::Core::Singleton<LogManager>
	{
		friend class Pitaya::Core::Singleton<LogManager>;
	private:
		struct LogMessage
		{
			LogLevel level;
			std::string time;
			std::string message;
			std::string thread;
		};

	private:
		LogManager()
		{
			const std::filesystem::path path = std::filesystem::path(Pitaya::Core::Utility::GetExeDir()) / fileName;
			ofs.open(path, std::ios::out | std::ios::trunc);
			if (!ofs.is_open())
			{
				throw std::runtime_error("Open Log File Fail! Path: " + path.string());
			}
			logThread = std::thread(&LogManager::LogThread, this);
		}
		~LogManager()
		{
			isRunning = false;
			cond.notify_one();
			if (logThread.joinable())
			{
				logThread.join();
			}
			ofs.close();
			while (!queue.empty())
			{
				queue.pop();
			}
		}

	public:
		LogManager(const LogManager&) = delete;
		LogManager& operator=(const LogManager&) = delete;
		LogManager(LogManager&&) = delete;
		LogManager& operator=(LogManager&&) = delete;

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
		std::atomic<bool> isRunning = true;
		std::thread logThread;
		static inline constexpr const char* fileName = "log.txt";
	};
}