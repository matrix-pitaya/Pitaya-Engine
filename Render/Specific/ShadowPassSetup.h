#pragma once

#include<glm.hpp>
#include<cstdint>

namespace Pitaya::Render
{
	struct ShadowPassSetup
	{
		uint32_t LightType = 0;    // 0=Dir, 1=Point, 2=Spot
		uint32_t Layer = 0;
		glm::mat4 ShadowViewProjection = glm::mat4(1.0f);
	};
}
