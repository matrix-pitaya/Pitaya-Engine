#pragma once

#include<Core/Allocate/Allocate.h>
#include<Context/Common/Module.h>
#include<Event/Common/Event.h>
#include<Event/Common/FuncTable.h>

#include<functional>
#include<unordered_map>

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
		struct CallBack
		{
			void (*OnCallBack)(void*, const ::Pitaya::Event::Event&) = nullptr;
			void* listener = nullptr;
		};
		struct EventRegistry
		{
			std::vector<std::unordered_map<::Pitaya::Event::EventToken,CallBack>> Map;
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
		Pitaya::Event::EventToken Subscribe(Pitaya::Event::EventType type, void (*OnCallBack)(void*, const ::Pitaya::Event::Event&), void* listener) noexcept;
		bool UnSubscribe(const Pitaya::Event::EventToken& eventToken) noexcept;
		void Emit(const Pitaya::Event::Event& event) noexcept;

	private:
		EventRegistry registry;
	};
}
