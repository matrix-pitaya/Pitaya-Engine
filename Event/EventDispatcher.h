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
		public:
			inline void Clear() noexcept
			{
				for (auto& map : maps)
				{
					map.clear();
				}
			}
			inline bool Emplace(EventType type, EventToken token, CallBack callback) noexcept
			{
				return maps[static_cast<uint8_t>(type)].emplace(token, callback).second;
			}
			inline bool Contains(EventToken token)
			{
				return maps[static_cast<uint8_t>(token.type)].contains(token);
			}
			inline bool Erase(EventToken token)
			{
				return maps[static_cast<uint8_t>(token.type)].erase(token) > 0;
			}
			template<typename Func>
			inline void ForEach(EventType type, Func func)
			{
				for (const auto& pair : maps[static_cast<uint8_t>(type)])
				{
					func(pair.first, pair.second);
				}
			}

		private:
			std::unordered_map<EventToken, CallBack> maps[static_cast<uint8_t>(EventType::Invalid)] = {};
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
