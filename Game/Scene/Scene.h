#pragma once

#include<Game/Scene/SceneECS.h>
#include<Game/Scene/SceneSetting.h>

namespace Pitaya::Game
{
	struct Scene
	{
		SceneECS ECS;
		SceneSetting Setting;
	};
}
