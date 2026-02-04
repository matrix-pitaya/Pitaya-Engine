#pragma once

#include"Identifier/UID.h"
#include"ObjectPool/ObjectPool.h"

#include"GameObject.h"
#include"Transform.h"

#include"entt/entt.hpp"

namespace Pitaya::Engine
{
	class Scene
	{
	private:
		Scene() = default;
		~Scene() = default;

    public:
        GameObject* CreateGameObject(const std::string& name = "GameObject")
        {
            GameObject* gameobject = gameobjectPool.Get();
            entt::entity entity = ecsRegistry.create();

            gameobject->SetUID(Pitaya::Core::UID::Next());
            gameobject->SetEntityId(entity);
            gameobject->SetName(name);
            gameobject->SetScene(this);

            ecsRegistry.emplace<Transform>(entity, gameobject);
            return gameobject;
        }
        void RemoveGameObject(GameObject* gameobject)
        {
            if (!gameobject || !gameobject->IsValid() || !ecsRegistry.valid(gameobject->GetEntityId()))
            {
                return;
            }

            //TODO 移除对象函数修改
            auto children = gameobject->GetChildren();
            for (auto child : children)
            {
                if (child)
                {
                    RemoveGameObject(child);
                }
            }

            //TODO 
            GameObject* parent = gameobject->GetParent();
            if (parent)
            {
                parent->RemoveChild(gameobject);
            }

            ecsRegistry.destroy(gameobject->GetEntityId());
            gameobjectPool.Release(gameobject);
        }
        template<typename T, typename... Args>
        T* AddComponent(GameObject* gameobject, Args&&... args)
        {
            return (gameobject && gameobject->IsValid() && ecsRegistry.valid(gameobject->GetEntityId())) ? 
                &ecsRegistry.get_or_emplace<T>(gameobject->GetEntityId(), std::forward<Args>(args)...) : nullptr;
        }
        template<typename T>
        void RemoveComponent(GameObject* gameobject)
        {
            if (!gameobject || !gameobject->IsValid() || !ecsRegistry.valid(gameobject->GetEntityId()))
            {
                return;
            }

            ecsRegistry.erase<T>(gameobject->GetEntityId());
        }
        template<typename T>
        T* GetComponent(GameObject* gameobject) const
        {
            return (gameobject && gameobject->IsValid() && ecsRegistry.valid(gameobject->GetEntityId())) ? 
                ecsRegistry.try_get<T>(gameobject->GetEntityId()) : nullptr;
        }
        template<typename... T, typename Func>
        void ViewForEach(Func&& func)
        {
            ecsRegistry.view<T...>().each(std::forward<Func>(func));
        }
        template<typename... Get, typename... Exclude, typename Func>
        void ViewForEachWithFiltered(entt::exclude_t<Exclude...> ex, Func&& func)
        {
            ecsRegistry.view<Get...>(ex).each(std::forward<Func>(func));
        }

	private:
		entt::registry ecsRegistry;
        Pitaya::Core::ObjectPool<GameObject> gameobjectPool = Pitaya::Core::ObjectPool<GameObject>([](GameObject* gameobject) {gameobject->Reset(); },
            [](GameObject* gameobject) {gameobject->Reset(); }, 30);
	};
}