#pragma once

#include"Enum/Enum.h"

#include"Object.h"

#include"entt/entt.hpp"

#include<vector>

namespace Pitaya::Engine
{
	class Scene;
	class GameObject : public Object
	{
		DEFAULT_COPY_AND_MOVE_PRIVATE(GameObject)

		friend entt::storage<GameObject>;
	private:
		GameObject(Scene* scene, const std::string& name = "GameObject", entt::entity entityId = entt::null)
			:scene(scene), name(name) , entityId(entityId){}
		~GameObject() override = default;

	public:
		inline std::vector<GameObject*> GetChildren()
		{
			//TODO 通过Transform实现
			return {};
		}
		inline entt::entity GetEntityId() const noexcept
		{
			return entityId;
		}
		inline void SetEntityId(entt::entity entityId) noexcept
		{
			this->entityId = entityId;
		}

	private:
		std::string name = "GameObject";
		Scene* scene = nullptr;
		entt::entity entityId = entt::null;
	};
}

namespace entt 
{
	template<> 
	struct component_traits<Pitaya::Engine::GameObject> 
	{
		static constexpr bool in_place_delete = true; 
	}; 
}