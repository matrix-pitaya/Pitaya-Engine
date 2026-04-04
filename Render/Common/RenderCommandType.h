#pragma once

#include<cstdint>

namespace Pitaya::Render
{
	enum class RenderCommandType : uint8_t
	{
		BeginPass = 0,
		InstanceDraw,
		PostProcess,

		Unknown
	};
}
