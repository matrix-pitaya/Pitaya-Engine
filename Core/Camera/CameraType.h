#pragma once

#include<cstdint>

namespace Pitaya::Core
{
	enum class CameraType : uint8_t
	{
		Perspective = 0,
		Orthographic
	};
}
