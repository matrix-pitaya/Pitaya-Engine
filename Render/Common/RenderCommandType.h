#pragma once

#include<cstdint>

namespace Pitaya::Render
{
	enum class RenderCommandType : uint32_t
	{
		BeginPass = 0,
		InstanceDraw,
		DrawSkybox,
		PostProcess,
		BlitToScreen,

		Unknown
	};
}
