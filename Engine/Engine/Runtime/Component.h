#pragma once

#include"Enum/Enum.h"

#include"Object.h"

#include"entt/entt.hpp"

namespace Pitaya::Engine
{
	class GameObject;
	class Component : public Object
	{
		DEFAULT_COPY_AND_MOVE_PUBLIC(Component)

		friend entt::storage<Component>;
		friend struct entt::component_traits<Component>;
	public:
		Component(GameObject* gameobject)
			: gameobject(gameobject) { }
		virtual ~Component() override = default;

	protected:
		GameObject* gameobject = nullptr;
	};
}
