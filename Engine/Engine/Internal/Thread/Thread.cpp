#include<Engine/Internal/Thread/Thread.h>

bool Pitaya::Engine::Internal::Thread::Initialize()
{
	//注册主线程
	map.emplace(Pitaya::Engine::Thread::ThreadToken(std::this_thread::get_id()), ThreadInfo("Main"));
	return true;
}
void Pitaya::Engine::Internal::Thread::Release()
{
	//通过容器中的ThreadInfo回收线程
	auto iterator = map.begin();
	while (iterator != map.end())
	{
		if (iterator->second.name == "Main" || iterator->second.name == "Log")
		{
			iterator++;
			continue;
		}

		iterator = map.erase(iterator);
	}
}

Pitaya::Engine::Internal::Thread::ThreadInfo::~ThreadInfo()
{
	if (thread.has_value() && thread->joinable())
	{
		thread->join();
		Pitaya::Engine::Log::LogDebug(name + "线程被回收");
	}
}