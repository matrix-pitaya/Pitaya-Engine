#pragma once

#include<Event/Common/Event.h>

namespace Pitaya::Event
{
	struct EventCallBack
	{
		void (*OnCallBack)(void*, Event) = nullptr;
		void* listener = nullptr;
	};
}
