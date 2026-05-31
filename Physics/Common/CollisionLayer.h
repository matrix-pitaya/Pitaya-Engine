#pragma once

#include<cstdint>
#include<string_view>

namespace Pitaya::Physics
{
	enum class CollisionLayer : uint32_t
	{
		Empty   = 0,
		Default = 1 << 0,
		All     = 0xFFFFFFFF,
	};

	inline std::string_view ToString(CollisionLayer layer) noexcept
	{
		switch (layer)
		{
			case CollisionLayer::Empty:			return "Empty";
			case CollisionLayer::Default:		return "Default";
			case CollisionLayer::All:			return "All";
			default:							return "Unknown";
		}
	}
}
