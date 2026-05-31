#pragma once

#include<Core/Asset/AssetRef.h>
#include<Asset/Common/SkyBox.h>
#include<glm.hpp>

namespace Pitaya::Game
{
	struct SceneSetting
	{
		Pitaya::Core::AssetRef<Pitaya::Asset::SkyBox> SkyBox = nullptr;
		glm::vec3 AmbientColor = glm::vec3(0.2f);
	};
}
