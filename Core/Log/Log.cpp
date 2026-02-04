#include"Log.h"

#include"../Utility/Utility.h"
#include"../Thread/Threadapi.h"

void Pitaya::Core::Log::LogManager::LogThread()
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
void Pitaya::Core::Log::LogManager::Log(LogLevel level, const std::string& message)
{
	LogMessage logMessage;
	logMessage.level = level;
	logMessage.message = message;
	logMessage.time = Pitaya::Core::Utility::Date();
	logMessage.thread = Pitaya::Core::Thread::GetThreadName(std::this_thread::get_id());
	{
		std::lock_guard<std::mutex> lock(mutex);
		queue.push(logMessage);
	}
	cond.notify_one();
}