#include<Game/Entity/GameObjectTreeView.h>
#include<Game/Scene/Scene.h>
#include<Game/Component/Transform.h>
#include<Game/Component/ChildLink .h>

Pitaya::Game::GameObjectTreeView::Iterator& Pitaya::Game::GameObjectTreeView::Iterator::operator++()
{
    if (currentId != entt::null && scene)
    {
        auto* link = scene->GetComponent<ChildLink>(currentId);
        currentId = link ? link->GetNextSibling() : entt::null;
    }
    return *this;
}
Pitaya::Game::GameObject* Pitaya::Game::GameObjectTreeView::Iterator::operator*() const
{
    return nullptr;
}
