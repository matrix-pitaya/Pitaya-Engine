#pragma once

#include<Core/Layout/def.h>
#include<glm.hpp>

namespace Pitaya::Render
{
	struct STD140_LAYOUT InstanceInfo
	{
		glm::mat4 Model;
		glm::mat4 Normal;
		glm::uvec4 Params;	// x: ReceiveShadow, y: MaterialByteOffset, zw: unused
	};
}