#pragma once

#include<Core/Allocate/Allocate.h>
#include<Core/Thread/Thread.h>
#include<Log/Common/LogLevel.h>
#include<Context/Common/Module.h>

#include<fstream>
#include<sstream>
#include<string>
#include<chrono>
#include<iomanip>
#include<mutex>
#include<condition_variable>
#include<atomic>
#include<filesystem>
#include<ctime>
#include<cstddef>

namespace Pitaya::Log
{
	class Logger
	{
	public:
		struct Factory
		{
			friend class Pitaya::Engine::Module<Logger>;
		private:
			static Logger* Create()
			{
				return PITAYA_NEW(Logger);
			}
			static void Destroy(Logger* logger)
			{
				PITAYA_DELETE(logger);
			}
		};
		struct Lifecycle
		{
			friend class Pitaya::Engine::Module<Logger>;
		private:
			static bool Initialize(Logger* logger)
			{
				return logger->Initialize();
			}
			static void Release(Logger* logger)
			{
				logger->Release();
			}
		};

	private:
		struct [[deprecated("Use Byte Stream")]] LogMessage
		{
			time_t time;
			char message[1024];
			Pitaya::Core::Thread::Identifier thread;
			Pitaya::Log::LogLevel level;
		};

	private:
		Logger() = default;
		~Logger() = default;

	public:
		Logger(const Logger&) = delete;
		Logger& operator=(const Logger&) = delete;
		Logger(Logger&&) = delete;
		Logger& operator=(Logger&&) = delete;

	private:
		bool Initialize();
		void Release();

	public:
		void Write(Pitaya::Log::LogLevel level, std::string_view message);

	private:
		inline static void BootstrapLogThread(void* log, void*)
		{
			static_cast<Pitaya::Log::Logger*>(log)->LogThread();
		}
		void LogThread();

	private:
		void ParseBuffer(const std::vector<std::byte>& buffer);
		void Flush();

	private:
		std::ofstream ofs;
		std::mutex mutex;
		std::vector<std::byte> front;
		std::vector<std::byte> back;
		std::condition_variable cond;
		std::atomic<bool> isRunning = false;
		Pitaya::Core::Thread::Identifier logThread;
		uint32_t frontMessageCount = 0;

		inline static constexpr const char* fileName = "log.txt";
		inline static constexpr const uint32_t WakeupLogCountThreshold = 32;
		inline static constexpr const std::chrono::milliseconds WaitTimeoutThreshold = std::chrono::milliseconds(10);
	};
}
