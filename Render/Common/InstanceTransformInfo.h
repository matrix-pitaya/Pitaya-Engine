#pragma once

#include<Core/Layout/def.h>
#include<glm.hpp>

namespace Pitaya::Render
{
	struct STD140_LAYOUT InstanceTransformInfo
	{
		glm::mat4 Model;
		glm::mat4 Normal;
	};
}