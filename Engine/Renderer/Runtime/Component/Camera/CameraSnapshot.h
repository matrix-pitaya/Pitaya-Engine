#pragma once

#include<glm.hpp>

namespace Pitaya::Engine::Renderer
{
	struct CameraSnapshot
	{
		glm::mat4 View;
		glm::mat4 Projection;
		glm::mat4 ViewProjection;
		glm::vec3 Position;
	};
}