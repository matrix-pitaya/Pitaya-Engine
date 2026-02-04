#pragma once

#include"Enum/Enum.h"

#include"Object.h"

#include"entt/entt.hpp"

namespace Pitaya::Engine
{
	class GameObject;
	class Component : public Object
	{
	public:
		Component(GameObject* gameobject)
			: gameobject(gameobject) { }
		~Component() = default;

	protected:
		GameObject* gameobject = nullptr;
	};
}
