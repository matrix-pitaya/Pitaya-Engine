#pragma once

#include"../Define/Define.h"
#include"../Log/Logapi.h"

#include<string>
#include<unordered_map>
#include<optional>
#include<thread>
#include<sstream>

namespace Pitaya::Core::Thread
{
	struct ThreadToken
	{
		friend class ThreadManager;

		ThreadToken() = default;
		ThreadToken(std::thread::id id)
			:id(id) {}

		bool operator==(const ThreadToken& other) const noexcept
		{
			return id == other.id;
		}

		std::thread::id id;

	private:
		struct Hash
		{
			size_t operator()(const ThreadToken& taskToken) const noexcept
			{
				return std::hash<std::thread::id>()(taskToken.id);
			}
		};
	};

	class ThreadManager
	{
		DECLARE_SINGLETON_CLASS_R(ThreadManager)

	private:
		struct ThreadInfo
		{
			template<class Function, class... Args>
			ThreadInfo(const std::string& name, Function&& f, Args&&... args)
				:name(name), thread(std::in_place, std::forward<Function>(f), std::forward<Args>(args)...) { }
			ThreadInfo(const std::string& name)
				:name(name), thread(std::nullopt) { }
			~ThreadInfo()
			{
				if (thread.has_value() && thread->joinable())
				{
					thread->join();
					Core::Log::Log(Core::Log::LogLevel::Debug, name + "线程被回收");
				}
			}
			ThreadInfo(const ThreadInfo&) = delete;
			ThreadInfo& operator=(const ThreadInfo&) = delete;
			ThreadInfo(ThreadInfo&&) noexcept = default;
			ThreadInfo& operator=(ThreadInfo&&) noexcept = default;

			std::string name;
			std::optional<std::thread> thread;
		};

	public:
		template<class Function, class... Args>
		inline ThreadToken RegisterThread(const std::string& name, Function&& f, Args&&... args)
		{
			Core::Log::Log(Core::Log::LogLevel::Info, name + "线程被注册!");
			ThreadInfo threadInfo = ThreadInfo(name, std::forward<Function>(f), std::forward<Args>(args)...);
			ThreadToken threadToken = ThreadToken(threadInfo.thread ? threadInfo.thread->get_id() : std::thread::id());
			map.emplace(threadToken, std::move(threadInfo));
			return threadToken;
		}
		inline ThreadToken RegisterThread(const std::string& name, std::thread::id id)
		{
			Core::Log::Log(Core::Log::LogLevel::Info, name + "线程被注册!");
			ThreadInfo threadInfo = ThreadInfo(name);
			ThreadToken threadToken = ThreadToken(id);
			map.emplace(threadToken, std::move(threadInfo));
			return threadToken;
		}
		inline bool UnregisterThread(ThreadToken threadToken)
		{
			auto iterator = map.find(threadToken);
			if (iterator == map.end() || !iterator->second.thread)
			{
				return false;
			}

			if (iterator->second.thread->joinable())
			{
				iterator->second.thread->join();
				Core::Log::Log(Core::Log::LogLevel::Debug, iterator->second.name + "线程被回收");
			}
			map.erase(iterator);
			return true;
		}
		inline std::string GetThreadName(const std::thread::id& id)
		{
			auto iterator = map.find(Core::Thread::ThreadToken(id));
			if (iterator == map.end())
			{
				return "未注册线程 Id:" + (std::ostringstream()<< id).str();
			}

			return iterator->second.name;
		}

	private:
		std::unordered_map<ThreadToken, ThreadInfo,Core::Thread::ThreadToken::Hash> map;
	};
}