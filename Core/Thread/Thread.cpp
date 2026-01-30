#include"thread.h"

IMPLEMENT_SINGLETON_CLASS(Pitaya::Core::Thread, ThreadManager)

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