#pragma once

#include<cstdint>

namespace Pitaya::Serialize
{
	enum class API : uint8_t
	{
		YAML = 0,

		Unknown
	};
}
