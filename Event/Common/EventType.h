#pragma once

#include<cstdint>

namespace Pitaya::Event
{
	enum class EventType : uint8_t
	{
		System = 0,

		//Window
		WindowClose,
		WindowFramebufferResetSize,
		DropFile,

		//Input
		MouseCurrsorMove,
		MouseScroll,
		MouseButton,
		Key,

		Invalid,
	};
}
