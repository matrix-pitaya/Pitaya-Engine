#pragma once

#include<Core/Container/SlotMap.h>
#include<Event/Common/EventType.h>

#include<cstdint>
#include<atomic>

namespace Pitaya::Event
{
	struct EventCallBack;
	struct EventToken
	{
		EventToken(EventType type = EventType::Invalid, Pitaya::Core::SlotMap<EventCallBack>::Handle handle = Pitaya::Core::SlotMap<EventCallBack>::Handle::Invalid)
			: type(type), handle(handle) { }

		bool operator==(const EventToken& other) const noexcept
		{
			return handle == other.handle && type == other.type;
		}

		inline auto Handle() const noexcept
		{
			return handle;
		}
		inline auto Type() const noexcept
		{
			return type;
		}

	private:
		Pitaya::Core::SlotMap<EventCallBack>::Handle handle;
		Pitaya::Event::EventType type;
	};
}
