#pragma once

#include<entt/entt.hpp>

namespace Pitaya::Game
{
	struct Parent
	{
	public:
		inline void SetId(entt::entity parentId) noexcept
		{
			this->id = parentId;
		}
		inline entt::entity GetId() const noexcept
		{
			return id;
		}

	private:
		entt::entity id = entt::null;
	};
}
