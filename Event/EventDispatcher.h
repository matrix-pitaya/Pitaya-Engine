#pragma once

#include<Core/Allocate/Allocate.h>
#include<Context/Common/Module.h>
#include<Event/Common/Event.h>
#include<Event/Common/EventToken.h>
#include<Event/Common/EventCallBack.h>
#include<Event/Common/FuncTable.h>

#include<functional>

namespace Pitaya::Event
{
	class EventDispatcher
	{
	public:
		struct Factory
		{
			friend class Pitaya::Engine::Module<EventDispatcher>;
		private:
			static EventDispatcher* Create()
			{
				return PITAYA_NEW(EventDispatcher);
			}
			static void Destroy(EventDispatcher* eventDispatcher)
			{
				PITAYA_DELETE(eventDispatcher);
			}
		};
		struct Lifecycle
		{
			friend class Pitaya::Engine::Module<EventDispatcher>;
		private:
			static bool Initialize(EventDispatcher* eventDispatcher)
			{
				return eventDispatcher->Initialize();
			}
			static void Release(EventDispatcher* eventDispatcher)
			{
				eventDispatcher->Release();
			}
		};

	private:
		struct EventRegistry
		{
		public:
			inline void Clear() noexcept
			{
				for (auto& map : maps)
				{
					map.Clear();
				}
			}
			inline auto Emplace(EventType type, EventCallBack callback) noexcept
			{
				return maps[static_cast<uint32_t>(type)].Emplace(std::move(callback));
			}
			inline bool Contains(EventType type, Pitaya::Core::SlotMap<EventCallBack>::Handle handle) const noexcept
			{
				return maps[static_cast<uint32_t>(type)].Get(handle) != nullptr;
			}
			inline bool Erase(EventType type, Pitaya::Core::SlotMap<EventCallBack>::Handle handle) noexcept
			{
				return maps[static_cast<uint32_t>(type)].Remove(handle);
			}
			inline auto Each(EventType type) noexcept
			{
				return maps[static_cast<uint32_t>(type)].Each();
			}

		private:
			Pitaya::Core::SlotMap<EventCallBack> maps[static_cast<uint32_t>(EventType::Invalid)] = {};
		};

	private:
		EventDispatcher() = default;
		~EventDispatcher() = default;

	public:
		EventDispatcher(const EventDispatcher&) = delete;
		EventDispatcher& operator=(const EventDispatcher&) = delete;
		EventDispatcher(EventDispatcher&&) = delete;
		EventDispatcher& operator=(EventDispatcher&&) = delete;

	private:
		bool Initialize();
		void Release();

	public:
		EventToken Subscribe(EventType type, void (*OnCallBack)(void*, const Event&), void* listener) noexcept;
		bool UnSubscribe(EventToken eventToken) noexcept;
		void Emit(const Event& event) noexcept;

	private:
		EventRegistry registry;
	};
}
