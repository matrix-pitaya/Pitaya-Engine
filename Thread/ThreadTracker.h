#pragma once

#include<Core/Allocate/Allocate.h>
#include<Core/Thread/Thread.h>
#include<Log/Common/FuncTable.h>
#include<Thread/Common/ThreadType.h>

#include<string>
#include<unordered_map>
#include<vector>
#include<array>
#include<algorithm>
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
			ThreadInfo(ThreadType type, std::string_view name, Func&& func, Args&&... args)
				:type(type), name(name), thread(std::forward<Func>(func), std::forward<Args>(args)...) {}
			ThreadInfo(ThreadType type, std::string_view name)
				:type(type), name(name){}
			~ThreadInfo() = default;
			ThreadInfo(const ThreadInfo&) = delete;
			ThreadInfo& operator=(const ThreadInfo&) = delete;
			ThreadInfo(ThreadInfo&&) noexcept = default;
			ThreadInfo& operator=(ThreadInfo&&) noexcept = default;

			std::string name;
			Pitaya::Thread::ThreadType type = ThreadType::Invalid;
			Pitaya::Core::Thread thread;
		};
		struct ThreadRegistry
		{
		private:
			struct ThreadIdStore
			{
				inline bool Emplace(Pitaya::Core::Thread::Identifier id, Pitaya::Thread::ThreadType type) noexcept
				{
					if (Pitaya::Thread::GetIsPrimaryThreadType(type)) { PrimaryThreadIds[static_cast<size_t>(type)] = id; return true; }
					if (type == Pitaya::Thread::ThreadType::Job) { JobThreadIds.push_back(id); return true; }
					return false;
				}
				inline bool Erase(Pitaya::Core::Thread::Identifier id, Pitaya::Thread::ThreadType type) noexcept
				{
					if (Pitaya::Thread::GetIsPrimaryThreadType(type)) { PrimaryThreadIds[static_cast<size_t>(type)] = Pitaya::Core::Thread::Identifier::Invalid; return true; }
					else if (type == Pitaya::Thread::ThreadType::Job) { auto it = std::find(JobThreadIds.begin(), JobThreadIds.end(), id); if (it != JobThreadIds.end()) { JobThreadIds.erase(it); } return true; }
					return false;
				}
				inline bool GetIsInThread(Pitaya::Thread::ThreadType type) const noexcept
				{
					if (type == ThreadType::Invalid) { return false; }

					Pitaya::Core::Thread::Identifier id = Pitaya::Core::Thread::GetCurrentThreadId();
					if (Pitaya::Thread::GetIsPrimaryThreadType(type)) { return id == PrimaryThreadIds[static_cast<size_t>(type)]; }
					
					if (type == Pitaya::Thread::ThreadType::Job)
					{
						for (auto jobId : JobThreadIds)
						{
							if (id == jobId) { return true; }
						}
					}
					return false;
				}

			private:
				std::array<Pitaya::Core::Thread::Identifier, PrimaryThreadTypeCount> PrimaryThreadIds;	//PrimaryThreadType → ThreadId
				std::vector<Pitaya::Core::Thread::Identifier> JobThreadIds;								//JobThreadType → ThreadId
			};
			struct ThreadInfoStore
			{
				struct View
				{
					View(ThreadInfoStore* store) 
						: store(store) {}

					inline auto begin() 
					{ 
						return store->map.begin();
					}
					inline auto end() 
					{
						return store->map.end();
					}

				private:
					ThreadInfoStore* store;
				};

				inline bool Emplace(Pitaya::Core::Thread::Identifier id, ThreadInfo&& info)
				{
					return map.emplace(id, std::move(info)).second;
				}
				inline bool Erase(Pitaya::Core::Thread::Identifier id)
				{
					return map.erase(id) > 0;
				}
				inline bool Contains(Pitaya::Core::Thread::Identifier id) const
				{
					return map.contains(id);
				}
				inline bool Join(Pitaya::Core::Thread::Identifier id)
				{
					return map.find(id)->second.thread.Join();
				}
				inline bool JoinWaitForMilliseconds(Pitaya::Core::Thread::Identifier id, std::chrono::milliseconds timeout)
				{
					return map.find(id)->second.thread.JoinWaitForMilliseconds(timeout);
				}
				inline bool GetThreadIsRunning(Pitaya::Core::Thread::Identifier id)
				{
					return map.find(id)->second.thread.IsRunning();
				}
				inline std::string_view GetThreadName(Pitaya::Core::Thread::Identifier id) const
				{
					return map.find(id)->second.name;
				}
				inline Pitaya::Thread::ThreadType GetThreadType(Pitaya::Core::Thread::Identifier id) const
				{
					return map.find(id)->second.type;
				}
				inline View Each() noexcept
				{
					return View{ this };
				}

			private:
				std::unordered_map<Pitaya::Core::Thread::Identifier, ThreadInfo> map;			//ThreadId → ThreadInfo
			};

		public:
			inline bool Emplace(Pitaya::Core::Thread::Identifier id, Pitaya::Thread::ThreadType type, ThreadInfo&& info)
			{
				return threadInfoStore.Emplace(id, std::move(info)) && threadIdStore.Emplace(id, type);
			}
			inline bool Erase(Pitaya::Core::Thread::Identifier id)
			{
				auto type = threadInfoStore.GetThreadType(id);
				return threadInfoStore.Erase(id) && threadIdStore.Erase(id, type);
			}
			inline bool Contains(Pitaya::Core::Thread::Identifier id) const
			{
				return threadInfoStore.Contains(id);
			}
			inline auto Each() noexcept
			{
				return threadInfoStore.Each();
			}
			inline bool Join(Pitaya::Core::Thread::Identifier id)
			{
				return threadInfoStore.Join(id);
			}
			inline bool JoinWaitForMilliseconds(Pitaya::Core::Thread::Identifier id, std::chrono::milliseconds timeout)
			{
				return threadInfoStore.JoinWaitForMilliseconds(id, timeout);
			}
			inline bool GetIsInThread(Pitaya::Thread::ThreadType type) const
			{
				return threadIdStore.GetIsInThread(type);
			}
			inline bool GetThreadIsRunning(Pitaya::Core::Thread::Identifier id)
			{
				return threadInfoStore.GetThreadIsRunning(id);
			}
			inline std::string_view GetThreadName(Pitaya::Core::Thread::Identifier id) const
			{
				return threadInfoStore.GetThreadName(id);
			}

		private:
			ThreadIdStore threadIdStore;
			ThreadInfoStore threadInfoStore;
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
		inline Pitaya::Core::Thread::Identifier RegisterThread(ThreadType type, std::string_view name, Func&& func, Args&&... args)
		{
			Pitaya::Thread::ThreadTracker::ThreadInfo threadInfo { type, name, std::forward<Func>(func), std::forward<Args>(args)... };
			Pitaya::Core::Thread::Identifier id = threadInfo.thread.GetThreadId();
			registry.Emplace(id, type, std::move(threadInfo));
			Pitaya::Log::Info("{} thread created and registered", name);
			return id;
		}
		inline bool UnregisterThread(Pitaya::Core::Thread::Identifier id)
		{
			if (!registry.Contains(id))
			{
				Pitaya::Log::Error("unregistered thread id: {}", static_cast<uint64_t>(id));
				return false;
			}

			std::string_view name = registry.GetThreadName(id);
			if (registry.Join(id))
			{
				Pitaya::Log::Info("{} thread join success", name);
				return registry.Erase(id);
			}

			Pitaya::Log::Error("{} thread join fail", name);
			return registry.Erase(id);
		}
		inline bool UnregisterThread(Pitaya::Core::Thread::Identifier id, std::chrono::milliseconds timeout)
		{
			if (!registry.Contains(id))
			{
				Pitaya::Log::Error("unregistered thread id: {}", static_cast<uint64_t>(id));
				return false;
			}

			std::string_view name = registry.GetThreadName(id);
			if (registry.JoinWaitForMilliseconds(id, timeout))
			{
				Pitaya::Log::Info("{} thread join success", name);
				return registry.Erase(id);
			}

			Pitaya::Log::Error("{} thread join timeout and terminated", name);
			return registry.Erase(id);
		}
		inline bool GetIsInThread(ThreadType type) const noexcept
		{
			return registry.GetIsInThread(type);
		}
		inline bool GetThreadIsRunning(Pitaya::Core::Thread::Identifier id)
		{
			if (!registry.Contains(id))
			{
				Pitaya::Log::Error("try find unregistered thread id: {}", static_cast<uint64_t>(id));
				return false;
			}
			return registry.GetThreadIsRunning(id);
		}
		inline std::string GetThreadName(Pitaya::Core::Thread::Identifier id)
		{
			return registry.Contains(id) ? std::string(registry.GetThreadName(id)) :
				"try find unregistered thread id:" + std::to_string(id);
		}

	private:
		ThreadRegistry registry;
	};
}
