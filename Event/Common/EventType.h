#pragma once

#include<cstdint>

namespace Pitaya::Event
{
	enum class EventType : uint32_t
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

		Invalid
	};

	inline constexpr const size_t EventTypeCount = static_cast<size_t>(EventType::Invalid);
}
