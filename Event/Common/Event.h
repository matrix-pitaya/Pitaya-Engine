#pragma once

#include<Event/Common/EventArgs.h>
#include<Event/Common/EventToken.h>

namespace Pitaya::Event
{
	struct Event
	{
		Event(Pitaya::Event::EventType type, const Pitaya::Event::EventArgs& args)
			:type(type), args(args) {}

		const Pitaya::Event::EventType type;
		const Pitaya::Event::EventArgs& args;
	};
}
