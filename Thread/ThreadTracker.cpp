#include<Thread/ThreadTracker.h>

bool Pitaya::Thread::ThreadTracker::Initialize()
{
	//注册主线程
	auto mainId = Pitaya::Core::Thread::GetCurrentThreadId();
	registry.Emplace(mainId, ThreadType::Main, ThreadInfo(ThreadType::Main, "Main"));
	return true;
}
void Pitaya::Thread::ThreadTracker::Release()
{
	for (auto& [id, info] : registry.Each())
	{
		if (info.type == ThreadType::Main || info.type == ThreadType::Log) { continue; }
		Pitaya::Log::Error(info.name + " not actively recycled, " +
			(info.thread.JoinWaitForMilliseconds(std::chrono::milliseconds(1000)) ? "join success" : "join timeout terminated"));
	}
}
