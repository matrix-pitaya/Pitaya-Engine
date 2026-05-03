#pragma once

#include<Render/Common/def.h>
#include<glm.hpp>

namespace Pitaya::Renderer
{
	struct STD140_LAYOUT CSMShadowGeneratePass
	{
		glm::mat4 LightVP;
	};
}