#pragma once

#include"Object.h"

#include<entt/entt.hpp>

namespace Pitaya::Engine
{
	class Entity : public Object
	{
	public:
		Entity(entt::entity entityId = entt::null)
			:entityId(entityId) {}
		~Entity() = default;

		inline entt::entity GetEntityId() const noexcept
		{
			return entityId;
		}
		inline void SetEntityId(entt::entity entityId) noexcept
		{
			this->entityId = entityId;
		}

	protected:
		entt::entity entityId = entt::null;
	};
}
