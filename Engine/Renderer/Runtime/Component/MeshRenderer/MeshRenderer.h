#pragma once

#include<Engine/Runtime/Component.h>

namespace Pitaya::Engine
{
	class MeshRenderer : public Component
	{
		MeshRenderer(GameObject* gameobject)
			:Component(gameobject) {}
		~MeshRenderer() = default;
	};
}