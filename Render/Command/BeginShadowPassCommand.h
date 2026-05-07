#pragma once

#include<Core/Container/SlotMap.h>
#include<GPU/Common/GPUObjectType.h>
#include<Render/Common/RenderCommandType.h>
#include<glm.hpp>
#include<cstdint>

namespace Pitaya::Render
{
	struct BeginShadowPassCommand
	{
		inline static constexpr const RenderCommandType Type = RenderCommandType::BeginShadowPass;

		uint32_t LightType = 0;    // 0=Dir, 1=Point, 2=Spot
		uint32_t Layer = 0;
		uint32_t Resolution = 2048;
		glm::mat4 ShadowViewProjection = glm::mat4(1.0f);
	};
}
