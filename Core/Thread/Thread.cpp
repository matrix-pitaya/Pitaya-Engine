#include"thread.h"

IMPLEMENT_SINGLETON_CLASS(Pitaya::Core::Thread, ThreadManager)

void Pitaya::Core::Thread::ThreadManager::Bootstrap()
{
	if (isBootstraped)
	{
		return;
	}

	//注册主线程
	map.emplace(ThreadToken(std::this_thread::get_id()), ThreadInfo("Main"));

	isBootstraped = true;
}
void Pitaya::Core::Thread::ThreadManager::Release()
{
	if (isReleased)
	{
		return;
	}

	//通过容器中的ThreadInfo回收线程
	map.clear();

	isReleased = true;
}