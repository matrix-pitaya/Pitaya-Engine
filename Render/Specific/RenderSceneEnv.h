#pragma once

#include<glm.hpp>

namespace Pitaya::Asset { struct SkyBox; }

namespace Pitaya::Render
{
	struct RenderSceneEnv
	{
		const Pitaya::Asset::SkyBox* SkyBox = nullptr;
		glm::vec3 AmbientColor = glm::vec3(1.0f);
	};
}
