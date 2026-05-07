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
		BeginShadowPass,

		Invalid
	};

	inline constexpr const size_t RenderCommandTypeCount = static_cast<size_t>(RenderCommandType::Invalid);
}
