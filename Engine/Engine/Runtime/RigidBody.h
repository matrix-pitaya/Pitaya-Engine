#pragma once

#include"Component.h"

namespace Pitaya::Engine
{
	class RigidBody : public Component
	{
		Default_Constructor_Destructor_Move_Copy(RigidBody)

	public:
		static constexpr const ComponentType Type = ComponentType::RigidBody;

	private:
		Physics::RigidBodyType type  = Physics::RigidBodyType::Static;
	};
}