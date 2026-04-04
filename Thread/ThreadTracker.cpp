#include<Thread/ThreadTracker.h>

bool Pitaya::Thread::ThreadTracker::Initialize()
{
	//注册主线程
	registry.Map.emplace(Pitaya::Core::Thread::GetCurrentThreadId(), ThreadInfo("Main"));
	return true;
}
void Pitaya::Thread::ThreadTracker::Release()
{
	//通过容器中的ThreadInfo回收线程
	auto iterator = registry.Map.begin();
	while (iterator != registry.Map.end())
	{
		if (iterator->second.name == "Main" || iterator->second.name == "Log")
		{
			iterator++;
			continue;
		}

		Pitaya::Log::Error(iterator->second.name + " not actively recycled, " +
			(iterator->second.thread.JoinWaitForMilliseconds(std::chrono::milliseconds(1000)) ? "join success" : "join timeout terminated"));
		iterator = registry.Map.erase(iterator);
	}
}
