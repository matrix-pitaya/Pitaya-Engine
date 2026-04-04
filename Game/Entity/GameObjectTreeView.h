#pragma once

#include<entt/entt.hpp>

namespace Pitaya::Game
{
    class Scene;
    class GameObject;

    //GameObject树的视图
    class GameObjectTreeView
    {
    public:
        struct Iterator
        {
            Scene* scene = nullptr;
            entt::entity currentId = entt::null;

            bool operator!=(const Iterator& other) const 
            { 
                return currentId != other.currentId; 
            }

            Iterator& operator++(); 
            GameObject* operator*() const;
        };

    public:
        GameObjectTreeView(Scene* scene, entt::entity first)
            : scene(scene), firstChild(first) {}

    public:
        inline Iterator begin() const 
        {
            return { scene, firstChild };
        }
        inline Iterator end() const 
        { 
            return { scene, entt::null };
        }

    private:
        Scene* scene = nullptr;
        entt::entity firstChild = entt::null;
    };
}
