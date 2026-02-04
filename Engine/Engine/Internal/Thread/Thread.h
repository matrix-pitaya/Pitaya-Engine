#pragma once

#include"../Engine/Engine/EngineAPI.h"

#include<string>
#include<unordered_map>
#include<optional>
#include<thread>
#include<sstream>

namespace Pitaya::Engine
{
	class Engine;
}

namespace Pitaya::Engine::Internal
{
	class Thread
	{
		friend class Pitaya::Engine::Engine;
	private:
		struct ThreadInfo
		{
			template<class Function, class... Args>
			ThreadInfo(const std::string& name, Function&& f, Args&&... args)
				:name(name), thread(std::in_place, std::forward<Function>(f), std::forward<Args>(args)...) {}
			ThreadInfo(const std::string& name)
				:name(name), thread(std::nullopt) {}
			~ThreadInfo();
			ThreadInfo(const ThreadInfo&) = delete;
			ThreadInfo& operator=(const ThreadInfo&) = delete;
			ThreadInfo(ThreadInfo&&) noexcept = default;
			ThreadInfo& operator=(ThreadInfo&&) noexcept = default;

			std::string name;
			std::optional<std::thread> thread;
		};

	private:
		Thread() = default;
		~Thread() = default;

	public:
		Thread(const Thread&) = delete;
		Thread& operator=(const Thread&) = delete;
		Thread(Thread&&) = delete;
		Thread& operator=(Thread&&) = delete;

	private:
		bool Initialize();
		void Release();

	private:
		template<class Function, class... Args>
		inline Pitaya::Engine::Thread::ThreadToken RegisterThread(const std::string& name, Function&& f, Args&&... args)
		{
			ThreadInfo threadInfo = ThreadInfo(name, std::forward<Function>(f), std::forward<Args>(args)...);
			Pitaya::Engine::Thread::ThreadToken threadToken = Pitaya::Engine::Thread::ThreadToken(threadInfo.thread ? threadInfo.thread->get_id() : std::thread::id());
			map.emplace(threadToken, std::move(threadInfo));
			return threadToken;
		}
		inline Pitaya::Engine::Thread::ThreadToken RegisterThread(const std::string& name, std::thread::id id)
		{
			ThreadInfo threadInfo = ThreadInfo(name);
			Pitaya::Engine::Thread::ThreadToken threadToken = Pitaya::Engine::Thread::ThreadToken(id);
			map.emplace(threadToken, std::move(threadInfo));
			return threadToken;
		}
		inline bool UnregisterThread(Pitaya::Engine::Thread::ThreadToken threadToken)
		{
			auto iterator = map.find(threadToken);
			if (iterator == map.end() || !iterator->second.thread)
			{
				return false;
			}

			if (iterator->second.thread->joinable())
			{
				iterator->second.thread->join();
			}
			map.erase(iterator);
			return true;
		}
		inline const std::string& GetThreadName(const std::thread::id& id)
		{
			auto iterator = map.find(Pitaya::Engine::Thread::ThreadToken(id));
			if (iterator == map.end())
			{
				return "Î´×¢²áÏß³Ì Id:" + (std::ostringstream() << id).str();
			}

			return iterator->second.name;
		}

	private:
		std::unordered_map<Pitaya::Engine::Thread::ThreadToken, ThreadInfo, Pitaya::Engine::Thread::ThreadToken::Hash> map;
	};
}