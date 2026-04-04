#pragma once

#include<cstdint>

namespace Pitaya::Engine
{
	enum class Lifecycle : uint8_t
	{
		BeginFrame = 0,
		FixedUpdate,
		Update,
		LateUpdate,
		EndFrame
	};
}