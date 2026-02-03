#pragma once

#include"Enum/Enum.h"

#include"Object.h"

#include"entt/entt.hpp"

namespace Pitaya::Engine
{
	class GameObject;
	class Component : public Object
	{
		DEFAULT_COPY_AND_MOVE_PROTECTED(Component)

		friend entt::storage<Component>;
	protected:
		Component(GameObject* gameobject)
			: gameobject(gameobject) { }
		virtual ~Component() override = default;

	protected:
		GameObject* gameobject = nullptr;
	};
}

namespace entt
{
	template<>
	struct component_traits<Pitaya::Engine::Component>
	{
		static constexpr bool in_place_delete = true;
	};
}