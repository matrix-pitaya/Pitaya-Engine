#pragma once

#include<cstdint>

namespace Pitaya::Game
{
	enum class ComponentType : uint8_t
	{
		Transform = 0,
		RigidBody,

		Unknown
	};
}
