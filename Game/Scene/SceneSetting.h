#pragma once

#include<Core/Asset/Asset.h>
#include<Asset/Common/SkyBox.h>
#include<glm.hpp>

namespace Pitaya::Game
{
	struct SceneSetting
	{
		Pitaya::Core::Asset<Pitaya::Asset::SkyBox> SkyBox = nullptr;
		glm::vec3 AmbientColor = glm::vec3(1.0f);
	};
}
