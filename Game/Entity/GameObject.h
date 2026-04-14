#pragma once

#include<Game/Entity/Entity.h>

namespace Pitaya::Game
{
    class Scene;
    class GameObject : public Pitaya::Game::Entity
    {
    public:
        GameObject(entt::entity id = entt::null, Scene* scene = nullptr)
            : Entity(id), scene(scene) {}
        ~GameObject() = default;

    public:
        inline bool IsValid() const noexcept
        { 
            return scene != nullptr && entityId != entt::null;
        }
        inline void Reset() noexcept
        {
            entityId = entt::null;
            scene = nullptr;
        }
        inline void SetScene(Scene* scene) noexcept
        {
            this->scene = scene;
        }
        inline Scene* GetScene() const noexcept 
        { 
            return scene; 
        }

    private:
        Scene* scene = nullptr;
    };
}
