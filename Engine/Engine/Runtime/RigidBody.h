#pragma once

#include"Component.h"

namespace Pitaya::Engine
{
	class RigidBody : public Component
	{
		DEFAULT_COPY_AND_MOVE_PUBLIC(RigidBody)

	public:
		RigidBody(GameObject* gameobject) 
			: Component(gameobject) {}
		~RigidBody() override = default;

	public:
		static constexpr const ComponentType Type = ComponentType::RigidBody;

	private:
		Physics::RigidBodyType type  = Physics::RigidBodyType::Static;
	};
}