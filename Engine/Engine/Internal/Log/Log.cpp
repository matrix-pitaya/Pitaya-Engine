#include<Engine/Internal/Log/Log.h>

#include<Engine/Internal/Thread/Thread.h>

bool Pitaya::Engine::Internal::Log::Initialize()
{
	const std::filesystem::path path = std::filesystem::path(Pitaya::Core::Utility::GetExeDir()) / fileName;
	ofs.open(path, std::ios::out | std::ios::trunc);
	if (!ofs.is_open())
	{
		throw std::runtime_error("Open Log File Fail! Path: " + path.string());
	}
	logThreadToken = Pitaya::Engine::Thread::GetThreadModel()->RegisterThread("Log", &Pitaya::Engine::Internal::Log::LogThread, this);
	return true;
}
void Pitaya::Engine::Internal::Log::Release()
{
	isRunning = false;
	cond.notify_one();
	Pitaya::Engine::Thread::GetThreadModel()->UnregisterThread(logThreadToken);
	while (!queue.empty())
	{
		const LogMessage& logMessage = queue.front();
		ofs << "[" << (logMessage.time) << "][" << LogLevelToString(logMessage.level) << "][" << logMessage.thread << "]" << logMessage.message << std::endl;
		queue.pop();
	}
	ofs.flush();
	ofs.close();
}
void Pitaya::Engine::Internal::Log::LogThread()
{
	while (true)
	{
		std::unique_lock<std::mutex> lock(mutex);
		cond.wait(lock, [this] { return !queue.empty() || !isRunning; });

		while (!queue.empty()) 
		{
			const LogMessage& logMessage = queue.front();
			ofs << "[" << (logMessage.time) << "][" << LogLevelToString(logMessage.level) << "][" << logMessage.thread << "]" << logMessage.message << std::endl;
			queue.pop();
		}
		ofs.flush();

		if (queue.empty() && !isRunning)
		{
			break;
		}
	}
}
void Pitaya::Engine::Internal::Log::LogInfo(const std::string& message)
{
	LogMessage logMessage;
	logMessage.level = Pitaya::Engine::Log::LogLevel::Info;
	logMessage.message = message;
	logMessage.time = Pitaya::Core::Utility::Date();
	logMessage.thread = Pitaya::Engine::Thread::GetThreadName(std::this_thread::get_id());
	{
		std::lock_guard<std::mutex> lock(mutex);
		queue.push(logMessage);
	}
	cond.notify_one();
}
void Pitaya::Engine::Internal::Log::LogDebug(const std::string& message)
{
	LogMessage logMessage;
	logMessage.level = Pitaya::Engine::Log::LogLevel::Debug;
	logMessage.message = message;
	logMessage.time = Pitaya::Core::Utility::Date();
	logMessage.thread = Pitaya::Engine::Thread::GetThreadName(std::this_thread::get_id());
	{
		std::lock_guard<std::mutex> lock(mutex);
		queue.push(logMessage);
	}
	cond.notify_one();
}
void Pitaya::Engine::Internal::Log::LogWarning(const std::string& message)
{
	LogMessage logMessage;
	logMessage.level = Pitaya::Engine::Log::LogLevel::Warning;
	logMessage.message = message;
	logMessage.time = Pitaya::Core::Utility::Date();
	logMessage.thread = Pitaya::Engine::Thread::GetThreadName(std::this_thread::get_id());
	{
		std::lock_guard<std::mutex> lock(mutex);
		queue.push(logMessage);
	}
	cond.notify_one();
}
void Pitaya::Engine::Internal::Log::LogError(const std::string& message)
{
	LogMessage logMessage;
	logMessage.level = Pitaya::Engine::Log::LogLevel::Error;
	logMessage.message = message;
	logMessage.time = Pitaya::Core::Utility::Date();
	logMessage.thread = Pitaya::Engine::Thread::GetThreadName(std::this_thread::get_id());
	{
		std::lock_guard<std::mutex> lock(mutex);
		queue.push(logMessage);
	}
	cond.notify_one();
}