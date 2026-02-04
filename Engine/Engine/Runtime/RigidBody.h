#pragma once

#include"Component.h"

namespace Pitaya::Engine
{
	class RigidBody : public Component
	{
	public:
		RigidBody(GameObject* gameobject) 
			: Component(gameobject) {}
		~RigidBody() = default;

	public:
		static constexpr const ComponentType Type = ComponentType::RigidBody;

	private:
		Physics::RigidBodyType type  = Physics::RigidBodyType::Static;
	};
}