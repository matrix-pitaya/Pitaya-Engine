#pragma once

#include<Event/Common/EventArgs.h>
#include<Event/Common/EventToken.h>

namespace Pitaya::Event
{
	struct Event
	{
		EventType type = EventType::Invalid;
		EventArgs args;
	};
}
