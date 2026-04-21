#pragma once

#include<cstdint>
#include<string_view>

namespace Pitaya::Physics
{
	enum class RigidBodyType : uint8_t
	{
		Static,
		Dynamic,
		Kinematic
	};

	inline std::string_view ToString(RigidBodyType type) noexcept
	{
		switch (type)
		{
			case RigidBodyType::Static:    return "Static";
			case RigidBodyType::Dynamic:   return "Dynamic";
			case RigidBodyType::Kinematic: return "Kinematic";
			default: return "Unknown";
		}
	}
}
