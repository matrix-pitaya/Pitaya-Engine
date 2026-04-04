#include<Task/TaskScheduler.h>
#include<Thread/Common/FuncTable.h>
#include<algorithm>

bool Pitaya::Task::TaskScheduler::Initialize()
{
	//获取CPU核心数量
	uint32_t cpuCoreCount = static_cast<uint32_t>(std::thread::hardware_concurrency());
	cpuCoreCount = cpuCoreCount == 0 ? 4 : cpuCoreCount;

	//线程数量 = CPU * 0.6 - 4, 最小值2 最大值核心数/2
	uint32_t jobExecuterCount = static_cast<uint32_t>(cpuCoreCount * 0.6f) - 5; //Main Log Renderer Physics System
	jobExecuterCount = std::clamp(jobExecuterCount, static_cast<uint32_t>(2), static_cast<uint32_t>(cpuCoreCount / 2));

	jobThreads.resize(jobExecuterCount);
	isRunning.store(true, std::memory_order_release);
	for (size_t i = 0; i < jobExecuterCount; i++)
	{
		jobThreads[i] = Pitaya::Thread::RegisterThread
		("JobThread_" + std::to_string(i + 1), &Pitaya::Task::TaskScheduler::BootstrapJobThread, this, nullptr);
	}
	return true;
}
void Pitaya::Task::TaskScheduler::Release()
{
	isRunning.store(false, std::memory_order_release);
	cond.notify_all();
	for (auto token : jobThreads)
	{
		Pitaya::Thread::UnregisterThread(token);
	}
	jobThreads.clear();
}
