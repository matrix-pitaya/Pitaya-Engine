#include<Thread/ThreadTracker.h>

bool Pitaya::Thread::ThreadTracker::Initialize()
{
	//注册主线程
	registry.Emplace(Pitaya::Core::Thread::GetCurrentThreadId(), ThreadInfo("Main"));
	return true;
}
void Pitaya::Thread::ThreadTracker::Release()
{
	//通过容器中的ThreadInfo回收线程
	registry.ForEach([](Pitaya::Core::Thread::Identifier id, ThreadInfo& info)
		{
			if (info.name == "Main" || info.name == "Log") { return; }
			Pitaya::Log::Error(info.name + " not actively recycled, " +
				(info.thread.JoinWaitForMilliseconds(std::chrono::milliseconds(1000)) ? "join success" : "join timeout terminated"));
		});
}
