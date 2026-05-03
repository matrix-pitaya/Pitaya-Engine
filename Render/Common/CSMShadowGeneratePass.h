#pragma once

#include<Core/Layout/def.h>
#include<glm.hpp>

namespace Pitaya::Renderer
{
	struct STD140_LAYOUT CSMShadowGeneratePass
	{
		glm::mat4 LightVP;
	};
}