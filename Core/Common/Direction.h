#pragma once

#include<cstdint>

namespace Pitaya::Core
{
	enum class Direction : uint8_t
	{
		Forward = 0,
		Back,
		Right,
		Left,
		Up,
		Down
	};
}
