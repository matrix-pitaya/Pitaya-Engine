#pragma once

#include"Component.h"

namespace Pitaya::Engine
{
	class RigidBody : public Component
	{
		DEFAULT_COPY_AND_MOVE_PRIVATE(RigidBody)

		friend entt::storage<RigidBody>;
	private:
		RigidBody(GameObject* gameobject) 
			: Component(gameobject) {}
		~RigidBody() override = default;

	public:
		static constexpr const ComponentType Type = ComponentType::RigidBody;

	private:
		Physics::RigidBodyType type  = Physics::RigidBodyType::Static;
	};
}

namespace entt
{
	template<>
	struct component_traits<Pitaya::Engine::RigidBody>
	{
		static constexpr bool in_place_delete = true;
	};
}