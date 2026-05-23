#pragma once

#include<Asset/Common/AssetType.h>
#include<glm.hpp>

namespace Pitaya::Render
{
	struct SceneEnv
	{
		const Pitaya::Asset::SkyBox* SkyBox = nullptr;
		glm::vec3 AmbientColor = glm::vec3(1.0f);
	};
}
