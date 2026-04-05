#include<Log/Logger.h>
#include<Thread/Common/FuncTable.h>
#include<Hook/def.h>
#include<Core/Utils/String.h>
#include<Core/Utils/Time.h>
#include<Core/Utils/File.h>

#include<cstring>

bool Pitaya::Log::Logger::Initialize()
{
	if (!Pitaya::Core::GenerateFile(fileName, "Pitaya Engine Log", "")) { throw std::runtime_error("Generate Log File Fail!"); }
	const std::filesystem::path path = Pitaya::Core::GetExecutableDirectory() / fileName;
	ofs.open(path, std::ios::out | std::ios::app);
	if (!ofs.is_open()) { throw std::runtime_error("Open Log File Fail! Path: " + path.string()); }
	front.reserve(1024 * 16);
	back.reserve(1024 * 16);
	isRunning.store(true, std::memory_order_release);
	logThread = Pitaya::Thread::RegisterThread("Log", &Pitaya::Log::Logger::BootstrapLogThread, this, nullptr);
	if (logThread == Pitaya::Core::Thread::Identifier::Invalid) { throw std::runtime_error("Log Thread Register Fail!"); }
	return true;
}
void Pitaya::Log::Logger::Release()
{
	isRunning.store(false, std::memory_order_release);
	cond.notify_one();
	Pitaya::Thread::UnregisterThread(logThread);
	Flush();
	ofs.close();
}
void Pitaya::Log::Logger::LogThread()
{
	while (true)
	{
		std::unique_lock<std::mutex> lock(mutex);
		if (!cond.wait_for(lock, WaitTimeoutThreshold,
			[this] { return !front.empty() || !isRunning.load(std::memory_order_acquire); })) { continue; }
		std::swap(front, back);
		frontMessageCount = 0;
		lock.unlock();

		//解锁打印日志
		if (!back.empty())
		{
			ParseBuffer(back);
			back.clear();
			ofs.flush();
		}

		//剩余日志由主线程打印
		if (!isRunning.load(std::memory_order_acquire)) { break; }
	}
}
void Pitaya::Log::Logger::Write(Pitaya::Log::LogLevel level, std::string_view message)
{
	if (message.empty()) { return; }

	const time_t currentTime = std::time(nullptr);
	const uint32_t messagelength = static_cast<uint32_t>(message.size());
	const Pitaya::Core::Thread::Identifier currentThreadId = Pitaya::Core::Thread::GetCurrentThreadId();

	{
		std::lock_guard<std::mutex> lock(mutex);

		const size_t neededBytes = sizeof(time_t) + sizeof(Pitaya::Core::Thread::Identifier) +
			sizeof(Pitaya::Log::LogLevel) + sizeof(uint32_t) + messagelength;

		const size_t currentSize = front.size();
		front.resize(currentSize + neededBytes);
		std::byte* writePtr = front.data() + currentSize;

		auto writeInfo = [&](const void* data, size_t size)
			{	
				std::memcpy(writePtr, data, size);
				writePtr += size;
			};

		writeInfo(&currentTime, sizeof(time_t));
		writeInfo(&currentThreadId, sizeof(Pitaya::Core::Thread::Identifier));
		writeInfo(&level, sizeof(Pitaya::Log::LogLevel));
		writeInfo(&messagelength, sizeof(uint32_t));
		writeInfo(message.data(), messagelength);

		if (++frontMessageCount >= WakeupLogCountThreshold)
		{
			cond.notify_one();
		}
	}

	INVOKE_POSTLOG_HOOK(level, message)
}
void Pitaya::Log::Logger::ParseBuffer(const std::vector<std::byte>& buffer)
{
	size_t offset = 0;
	const size_t totalSize = buffer.size();
	while (offset < totalSize)
	{
		time_t logTime;
		std::memcpy(&logTime, buffer.data() + offset, sizeof(time_t));
		offset += sizeof(time_t);

		Pitaya::Core::Thread::Identifier threadId;
		std::memcpy(&threadId, buffer.data() + offset, sizeof(Pitaya::Core::Thread::Identifier));
		offset += sizeof(Pitaya::Core::Thread::Identifier);

		Pitaya::Log::LogLevel level;
		std::memcpy(&level, buffer.data() + offset, sizeof(Pitaya::Log::LogLevel));
		offset += sizeof(Pitaya::Log::LogLevel);

		uint32_t msgLen;
		std::memcpy(&msgLen, buffer.data() + offset, sizeof(uint32_t));
		offset += sizeof(uint32_t);

		std::string_view msgText(reinterpret_cast<const char*>(buffer.data() + offset), msgLen);
		offset += msgLen;

		ofs << "[" << Pitaya::Core::Data(logTime) << "][" << ToString(level) << "][" <<
			Pitaya::Thread::GetThreadName(threadId) << "]" << msgText << "\n";
	}
}
void Pitaya::Log::Logger::Flush()
{
	{
		std::lock_guard<std::mutex> lock(mutex);
		ParseBuffer(back);
		ParseBuffer(front);
		back.clear();
		front.clear();
		frontMessageCount = 0;
	}
	ofs.flush();
}
