#pragma once

#include<Core/PassKey/PassKey.h>
#include<Core/Container/ObjectPool/ObjectPool.h>
#include<Game/Entity/GameObject.h>

#include<entt/entt.hpp>
#include<glm.hpp>

namespace Pitaya::Game
{
    class GameWorld;
    class GameObject;
	class Scene
	{
	private:
        Scene() = default;
		~Scene() = default;

    private:
        bool Initialize();
        void Release();

    public:
        GameObject* CreateGameObject(std::string_view name = "GameObject", std::string_view tag = "default");
        void RemoveGameObject(GameObject* gameobject);
        GameObject* GetGameObject(entt::entity entity);
        void SetRelationship(entt::entity childId, entt::entity parentId);

        template<typename... Components>
        inline auto GetView()
        {
            return ecsRegistry.view<Components...>();
        }
        template<typename T, typename... Args>
        inline T& AddComponent(entt::entity entity, Args&&... args)
        {
            return ecsRegistry.emplace<T>(entity, std::forward<Args>(args)...);
        }
        template<typename T>
        inline T* GetComponent(entt::entity entity)
        {
            return ecsRegistry.try_get<T>(entity);
        }
        template<typename T>
        inline bool HasComponent(entt::entity entity) const
        {
            return ecsRegistry.all_of<T>(entity);
        }
        template<typename T>
        inline void RemoveComponent(entt::entity entity)
        {
            ecsRegistry.remove<T>(entity);
        }

    private:
        void DestroyEntity(entt::entity entity);

    public:
        void ProcessTransformSystem(Pitaya::Core::PassKey<GameWorld>);

    private:
        void UpdateTransformHierarchy(entt::entity entity, const glm::mat4& parentWorldMatrix, bool parentChanged);

    private:
        static void ActionOnGet(GameObject*);
        static void ActionOnRelease(GameObject*);

	private:
		entt::registry ecsRegistry;
        Pitaya::Core::ObjectPool<GameObject> gameobjectPool { ActionOnGet, ActionOnRelease, 30 };
	};
}
