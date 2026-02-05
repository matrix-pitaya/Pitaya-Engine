#pragma once

#include<Enum/Enum.h>
#include<Utility/Utility.h>

#include<Engine/API/Thread.h>

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

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

namespace Pitaya::Engine
{
	class Engine;
}

namespace Pitaya::Engine::Internal
{
	class Log
	{
		friend class Pitaya::Engine::Engine;
	private:
		struct LogMessage
		{
			Pitaya::Engine::Log::LogLevel level;
			std::string time;
			std::string message;
			std::string thread;
		};

	private:
		Log() = default;
		~Log() = default;

	public:
		Log(const Log&) = delete;
		Log& operator=(const Log&) = delete;
		Log(Log&&) = delete;
		Log& operator=(Log&&) = delete;

	private:
		bool Initialize();
		void Release();

	public:
		void LogInfo(const std::string& message);
		void LogDebug(const std::string& message);
		void LogWarning(const std::string& message);
		void LogError(const std::string& message);

	private:
		void LogThread();
		inline std::string LogLevelToString(Pitaya::Engine::Log::LogLevel level)
		{
			switch (level)
			{
				case Pitaya::Engine::Log::LogLevel::Info:		return "Info";
				case Pitaya::Engine::Log::LogLevel::Debug:		return "Debug";
				case Pitaya::Engine::Log::LogLevel::Warning:	return "Warning";
				case Pitaya::Engine::Log::LogLevel::Error:		return "Error";
				default:										return "";
			}
		}

	private:
		std::ofstream ofs;
		std::mutex mutex;
		std::queue<LogMessage> queue;
		std::condition_variable cond;
		std::atomic<bool> isRunning = true;
		Pitaya::Engine::Thread::ThreadToken logThreadToken;

		static inline constexpr const char* fileName = "log.txt";
	};
}