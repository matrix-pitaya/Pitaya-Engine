#pragma once

#include<Core/Object/Object.h>

#include<GameCommon/ComponentType.h>

#include<entt/entt.hpp>

namespace Pitaya::Game
{
	class GameObject;
	class Component : public Pitaya::Core::Object
	{
	public:
		Component(GameObject* gameobject)
			: gameobject(gameobject) { }
		~Component() = default;

	protected:
		GameObject* gameobject = nullptr;
	};
}
