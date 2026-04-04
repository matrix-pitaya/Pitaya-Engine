#include<Game/Entity/GameObjectTreeView.h>
#include<Game/Scene/Scene.h>
#include<Game/Component/Transform.h>

Pitaya::Game::GameObjectTreeView::Iterator& Pitaya::Game::GameObjectTreeView::Iterator::operator++()
{
    if (currentId != entt::null && scene) 
    {
        auto* trans = scene->GetComponent<Transform>(currentId);
        currentId = trans ? trans->GetNextSibling() : entt::null;
    }
    return *this;
}
Pitaya::Game::GameObject* Pitaya::Game::GameObjectTreeView::Iterator::operator*() const
{
    return scene->GetGameObject(currentId);
}
