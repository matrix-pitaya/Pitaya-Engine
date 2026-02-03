#pragma once

#include"Define/Define.h"
#include"Identifier/UID.h"

#include"GameObject.h"
#include"Transform.h"

#include"entt/entt.hpp"

#include<unordered_map>

namespace Pitaya::Engine
{
    class Scene;
    class ECS
    {
        DELETE_COPY(ECS)

    public:
        entt::entity CreateGameObject(Scene* scene, const std::string& name = "GameObject")
        {
            entt::entity entity = ecsRegistry.create();
            GameObject* gameobject = &ecsRegistry.emplace<GameObject>(entity, scene, name, entity);
            ecsRegistry.emplace<Transform>(entity, gameobject);
            return entity;
        }
        void RemoveGameObject(entt::entity entityId)
        {
            if (!ecsRegistry.valid(entityId))
            {
                return;
            }

            GameObject* gameobject = ecsRegistry.try_get<GameObject>(entityId);
            if (!gameobject)
            {
                ecsRegistry.destroy(entityId);
                return;
            }

            //TODO 移除对象函数修改
            auto children = gameobject->GetChildren();
            for (auto child : children)
            {
                if (child)
                {
                    RemoveGameObject(child->GetEntityId());
                }
            }

            ecsRegistry.destroy(entityId);
        }
        template<typename T, typename... Args>
        T* AddComponent(entt::entity entityId, Args&&... args)
        {
            return ecsRegistry.valid(entityId) ? &ecsRegistry.get_or_emplace<T>(entityId, ecsRegistry.try_get<GameObject>(entityId), std::forward<Args>(args)...) : nullptr;
        }
        template<typename T>
        void RemoveComponent(entt::entity entityId)
        {
            if (!ecsRegistry.valid(entityId))
            {
                return;
            }

            ecsRegistry.remove_if_exists<T>(entityId);
        }
        template<typename T>
        T* GetComponent(entt::entity entityId)
        {
            return ecsRegistry.valid(entityId) ? ecsRegistry.try_get<T>(entityId) : nullptr;
        }
        template<typename... T, typename Func>
        void ViewForEach(Func&& func)
        {
            ecsRegistry.view<T...>().each(std::forward<Func>(func));
        }
        template<typename... T, typename Func>
        void ViewForEachWithGameObject(Func&& func)
        {
            ecsRegistry.view<GameObject, T...>().each([&](GameObject& go, T&... components) {func(&go, components...); });
        }
        template<typename... Get, typename... Exclude, typename Func>
        void ViewForEachWithFiltered(Func&& func)
        {
            ecsRegistry.view<Get...>(entt::exclude<Exclude...>).each(std::forward<Func>(func));
        }
        template<typename... Owned, typename... Observe, typename Func>
        void GroupForEach(Func&& func)
        {
            ecsRegistry.group<Owned...>(entt::get<Observe...>).each(std::forward<Func>(func));
        }

    private:
        entt::registry ecsRegistry;
    };
}
