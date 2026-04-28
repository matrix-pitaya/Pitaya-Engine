#pragma once

#include<glm.hpp>

namespace Pitaya::Render
{
	struct alignas(16) InstanceTransformInfo
	{
		glm::mat4 Model;
		glm::mat4 Normal;
	};
}