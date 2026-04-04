#pragma once

#include<Game/Component/Component.h>
#include<Game/Common/ComponentType.h>
#include<Physics/Common/RigidBodyType.h>

namespace Pitaya::Game
{
	class RigidBody : public Component
	{
	public:
		RigidBody(GameObject* gameobject) 
			: Component(gameobject) {}
		~RigidBody() = default;

	public:
		inline static constexpr const ComponentType Type = ComponentType::RigidBody;

	private:
		Physics::RigidBodyType type  = Physics::RigidBodyType::Static;
	};
}
