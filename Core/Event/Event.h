#pragma once

#include"../Enum/Enum.h"
#include"../Singleton/Singleton.h"

#include<functional>
#include<unordered_map>

namespace Pitaya::Core::Event
{
	namespace Args
	{
		struct EventArgs;
	}

	struct Event;
	struct EventToken;

	class EventManager : public Pitaya::Core::Singleton<EventManager>
	{
		friend class Pitaya::Core::Singleton<EventManager>;
	private:
		EventManager() = default;
		~EventManager() = default;

	public:
		EventManager(const EventManager&) = delete;
		EventManager& operator=(const EventManager&) = delete;
		EventManager(EventManager&&) = delete;
		EventManager& operator=(EventManager&&) = delete;

	public:
		Core::Event::EventToken Subscribe(::Pitaya::Core::Event::EventType type, std::function<void(const ::Pitaya::Core::Event::Event&)> function);
		bool UnSubscribe(const ::Pitaya::Core::Event::EventToken& eventToken);
		void Emit(const ::Pitaya::Core::Event::Event& event);

	private:
		std::unordered_map<::Pitaya::Core::Event::EventType,
			std::unordered_map<::Pitaya::Core::Event::EventToken, std::function<void(const ::Pitaya::Core::Event::Event&)>>> eventMap;
	};
}