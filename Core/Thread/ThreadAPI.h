#pragma once

#include"thread.h"

#include<string>

namespace Pitaya::Core::Thread
{
	template<class Function, class... Args>
	inline ThreadToken RegisterThread(const std::string& name, Function&& f, Args&&... args)
	{
		return Core::Thread::ThreadManager::Instance.RegisterThread(name, std::forward<Function>(f), std::forward<Args>(args)...);
	}
	inline ThreadToken RegisterThread(const std::string& name, std::thread::id id)
	{
		return Core::Thread::ThreadManager::Instance.RegisterThread(name, id);
	}
	inline bool UnregisterThread(ThreadToken threadToken)
	{
		return Core::Thread::ThreadManager::Instance.UnregisterThread(threadToken);;
	}
	inline std::string GetThreadName(const std::thread::id& id)
	{
		return Core::Thread::ThreadManager::Instance.GetThreadName(id);
	}
}