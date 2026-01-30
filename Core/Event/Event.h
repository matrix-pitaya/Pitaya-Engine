#pragma once

#include"../Enum/Enum.h"
#include"../Define/Define.h"

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

	class EventManager
	{
		DECLARE_SINGLETON_CLASS_R(EventManager)
	public:
		Core::Event::EventToken Subscribe(::Pitaya::Core::Event::EventType type, std::function<void(const ::Pitaya::Core::Event::Event&)> function);
		bool UnSubscribe(const ::Pitaya::Core::Event::EventToken& eventToken);
		void Emit(const ::Pitaya::Core::Event::Event& event);

	private:
		std::unordered_map<::Pitaya::Core::Event::EventType,
			std::unordered_map<::Pitaya::Core::Event::EventToken, std::function<void(const ::Pitaya::Core::Event::Event&)>>> eventMap;
	};
}