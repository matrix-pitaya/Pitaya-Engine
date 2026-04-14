#pragma once

#include<Physics/Common/RigidBodyType.h>

namespace Pitaya::Game
{
	struct RigidBody 
	{
	public:
		inline void SetType(Pitaya::Physics::RigidBodyType type) noexcept
		{
			this->type = type;
		}
		inline Pitaya::Physics::RigidBodyType GetType() const noexcept
		{
			return type;
		}

	private:
		Pitaya::Physics::RigidBodyType type  = Pitaya::Physics::RigidBodyType::Static;
	};
}
