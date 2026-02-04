#pragma once

#include"Enum/Enum.h"

#include"Entity.h"

#include<vector>

namespace Pitaya::Core
{
	template<typename T>
	class ObjectPool;
}

namespace Pitaya::Engine
{
	class Scene;
	class GameObject : public Entity
	{
	public:
		GameObject(entt::entity entityId = entt::null, const std::string& name = "GameObject", Scene* scene = nullptr)
			:Entity(entityId),scene(scene), name(name){}
		~GameObject() = default;

	public:
		inline bool IsValid()
		{
			return uid.IsValid() && (entityId != entt::null) && (scene != nullptr);
		}
		inline void Reset() noexcept
		{
			uid = Pitaya::Core::UID::Invalid;
			entityId = entt::null;
			name = "GameObject";
			scene = nullptr;

			//TODO 清空父节点、字节点
		}
		inline void SetName(const std::string& name) noexcept
		{
			this->name = name;
		}
		inline void SetScene(Scene* scene) noexcept
		{
			this->scene = scene;
		}
		
	public:
		inline GameObject* GetParent()
		{
			//TODO
			return nullptr;
		}
		inline std::vector<GameObject*> GetChildren()
		{
			//TODO 通过Transform实现
			return {};
		}
		inline bool RemoveChild(GameObject* child)
		{
			return true;
		}

	private:
		std::string name = "GameObject";
		Scene* scene = nullptr;
	};
}
