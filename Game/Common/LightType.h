#pragma once

#include<cstdint>

namespace Pitaya::Game
{
	enum class LightType : uint8_t
	{
		Directional = 0,
		Point = 1,
		Spot = 2
	};
}