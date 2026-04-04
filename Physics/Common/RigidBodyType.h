#pragma once

#include<cstdint>

namespace Pitaya::Physics
{
	enum class RigidBodyType : uint8_t
	{
		Static,
		Dynamic,
		Kinematic
	};
}
