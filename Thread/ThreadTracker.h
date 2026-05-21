#pragma once

#include<Core/Allocate/Allocate.h>
#include<Core/Thread/Thread.h>
#include<Log/Common/FuncTable.h>

#include<string>
#include<unordered_map>
#include<optional>
#include<thread>
#include<sstream>

namespace Pitaya::Thread
{
	class ThreadTracker
	{
	public:
		struct Factory
		{
			friend class Pitaya::Engine::Module<ThreadTracker>;
		private:
			static ThreadTracker* Create()
			{
				return PITAYA_NEW(ThreadTracker);
			}
			static void Destroy(ThreadTracker* threadTracker)
			{
				PITAYA_DELETE(threadTracker);
			}
		};
		struct Lifecycle
		{
			friend class Pitaya::Engine::Module<ThreadTracker>;
		private:
			static bool Initialize(ThreadTracker* threadTracker)
			{
				return threadTracker->Initialize();
			}
			static void Release(ThreadTracker* threadTracker)
			{
				threadTracker->Release();
			}
		};

	private:
		struct ThreadInfo
		{
			template<class Func, class... Args>
			ThreadInfo(std::string_view name, Func&& func, Args&&... args)
				:name(name), thread(std::forward<Func>(func), std::forward<Args>(args)...) {}
			ThreadInfo(std::string_view name)
				:name(name){}
			~ThreadInfo() = default;
			ThreadInfo(const ThreadInfo&) = delete;
			ThreadInfo& operator=(const ThreadInfo&) = delete;
			ThreadInfo(ThreadInfo&&) noexcept = default;
			ThreadInfo& operator=(ThreadInfo&&) noexcept = default;

			std::string name;
			Pitaya::Core::Thread thread;
		};
		struct ThreadRegistry
		{
		public:
			inline bool Emplace(Pitaya::Core::Thread::Identifier id, ThreadInfo&& info)
			{
				return map.emplace(id, std::move(info)).second;
			}
			inline auto Find(Pitaya::Core::Thread::Identifier id)
			{
				auto iterator = map.find(id);
				return std::make_pair(iterator, iterator != map.end());
			}
			inline auto Erase(auto iterator)
			{
				return map.erase(iterator);
			}
			template<typename Func>
			inline void ForEach(Func func)
			{
				for (auto& pair : map)
				{
					func(pair.first, pair.second);
				}
			}

		private:
			std::unordered_map<Pitaya::Core::Thread::Identifier, ThreadInfo> map;
		};

	private:
		ThreadTracker() = default;
		~ThreadTracker() = default;

	public:
		ThreadTracker(const ThreadTracker&) = delete;
		ThreadTracker& operator=(const ThreadTracker&) = delete;
		ThreadTracker(ThreadTracker&&) = delete;
		ThreadTracker& operator=(ThreadTracker&&) = delete;

	private:
		bool Initialize();
		void Release();

	public:
		template<class Func, class... Args>
		inline Pitaya::Core::Thread::Identifier RegisterThread(std::string_view name, Func&& func, Args&&... args)
		{
			ThreadInfo threadInfo { name, std::forward<Func>(func), std::forward<Args>(args)... };
			Pitaya::Core::Thread::Identifier id = threadInfo.thread.GetThreadId();
			registry.Emplace(id, std::move(threadInfo));
			Pitaya::Log::Info("{} thread created and registered", name);
			return id;
		}
		inline bool UnregisterThread(Pitaya::Core::Thread::Identifier id)
		{
			auto [iterator, valid] = registry.Find(id);
			if (!valid)
			{
				Pitaya::Log::Error("unregistered uhread id: {}", static_cast<uint64_t>(id));
				return false;
			}
			
			if (iterator->second.thread.Join())
			{
				Pitaya::Log::Info(GetThreadName(id) + " thread join success");
				registry.Erase(iterator);
				return true;
			}

			Pitaya::Log::Error(GetThreadName(id) + " thread join fail");
			registry.Erase(iterator);
			return true;
		}
		inline bool UnregisterThread(Pitaya::Core::Thread::Identifier id, const std::chrono::milliseconds& timeout)
		{
			auto [iterator, valid] = registry.Find(id);
			if (!valid)
			{
				Pitaya::Log::Error("unregistered uhread id: {}" , static_cast<uint64_t>(id));
				return false;
			}

			if (iterator->second.thread.JoinWaitForMilliseconds(timeout))
			{
				Pitaya::Log::Info(GetThreadName(id) + " thread join success");
				registry.Erase(iterator);
				return true;
			}

			Pitaya::Log::Error(GetThreadName(id) + " thread join timeout and terminated");
			registry.Erase(iterator);
			return false;
		}
		inline std::string GetThreadName(Pitaya::Core::Thread::Identifier id)
		{
			auto [iterator, valid] = registry.Find(id);
			return valid ? iterator->second.name :
				"try find unregistered thread id:" + std::to_string(id);
		}
		inline bool GetThreadIsRunning(Pitaya::Core::Thread::Identifier id)
		{
			auto [iterator, valid] = registry.Find(id);
			if (!valid)
			{
				Pitaya::Log::Error("try find unregistered thread id: {}" , static_cast<uint64_t>(id));
				return false;
			}
			return iterator->second.thread.IsRunning();
		}

	private:
		ThreadRegistry registry;
	};
}
