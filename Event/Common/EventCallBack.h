#pragma once

namespace Pitaya::Event
{
	struct Event;
	struct EventCallBack
	{
		void (*OnCallBack)(void*, const Event&) = nullptr;
		void* listener = nullptr;
	};
}
