#include<Game/Scene/Scene.h>
#include<Game/Component/Tag.h>
#include<Game/Component/Transform.h>
#include<Game/Entity/GameObject.h>

bool Pitaya::Game::Scene::Initialize()
{
    return true;
}
void Pitaya::Game::Scene::Release()
{

}

Pitaya::Game::GameObject* Pitaya::Game::Scene::CreateGameObject(std::string_view name, std::string_view tag)
{
    GameObject* gameobject = gameobjectPool.Get();
    entt::entity entity = ecsRegistry.create();

    gameobject->SetEntityId(entity);
    gameobject->SetScene(this);
    gameobject->SetUID(Pitaya::Core::UID::Next());

    AddComponent<Tag>(entity, name, tag, gameobject);
    AddComponent<Transform>(entity);

    return gameobject;
}
void Pitaya::Game::Scene::RemoveGameObject(Pitaya::Game::GameObject* gameobject)
{
    if (!gameobject || !gameobject->IsValid()) { return; }
    entt::entity id = gameobject->GetEntityId();
    SetRelationship(id, entt::null);
    DestroyEntity(id);
}
Pitaya::Game::GameObject* Pitaya::Game::Scene::GetGameObject(entt::entity entity)
{
    if (ecsRegistry.valid(entity)) 
    {
        if (auto* tag = GetComponent<Tag>(entity))
        {
            return tag->GetGameObject();
        }
    }
    return nullptr;
}
void Pitaya::Game::Scene::SetRelationship(entt::entity childId, entt::entity parentId)
{
    if (childId == parentId || !ecsRegistry.valid(childId)) { return; }

    auto* childTrans = GetComponent<Transform>(childId);
    if (!childTrans) { return; }

    //剥离
    if (childTrans->GetParent() != entt::null)
    {
        auto* oldParentTrans = GetComponent<Transform>(childTrans->GetParent());
        if (oldParentTrans && oldParentTrans->GetFirstChild() == childId)
        {
            oldParentTrans->SetFirstChild(childTrans->GetNextSibling());
        }
        if (childTrans->GetPreviousSibling() != entt::null)
        {
            GetComponent<Transform>(childTrans->GetPreviousSibling())->SetNextSibling(childTrans->GetNextSibling());
        }
        if (childTrans->GetNextSibling() != entt::null)
        {
            GetComponent<Transform>(childTrans->GetNextSibling())->SetPreviousSibling(childTrans->GetPreviousSibling());
        }
    }

    //挂载
    childTrans->SetParent(parentId);
    childTrans->SetPreviousSibling(entt::null);
    childTrans->SetNextSibling(entt::null);
    if (parentId != entt::null)
    {
        if (auto* newParentTrans = GetComponent<Transform>(parentId))
        {
            if (newParentTrans->GetFirstChild() != entt::null)
            {
                auto* oldFirst = GetComponent<Transform>(newParentTrans->GetFirstChild());
                if (oldFirst) { oldFirst->SetPreviousSibling(childId); }
                childTrans->SetNextSibling(newParentTrans->GetFirstChild());
            }
            newParentTrans->SetFirstChild(childId);
        }
    }

    if (auto* childTrans = GetComponent<Transform>(childId))
    {
        childTrans->SetWorldDirty(true);
    }
}
void Pitaya::Game::Scene::DestroyEntity(entt::entity entity)
{
    if (!ecsRegistry.valid(entity)) { return; }

    if (auto* trans = GetComponent<Transform>(entity))
    {
        entt::entity currChild = trans->GetFirstChild();
        while (currChild != entt::null)
        {
            entt::entity nextChild = GetComponent<Transform>(currChild)->GetNextSibling();
            DestroyEntity(currChild);
            currChild = nextChild;
        }
    }

    if (auto* tag = GetComponent<Tag>(entity))
    {
        if (GameObject* go = tag->GetGameObject())
        {
            gameobjectPool.Release(go);
        }
    }

    ecsRegistry.destroy(entity);
}
void Pitaya::Game::Scene::ProcessTransformSystem(Pitaya::Core::PassKey<GameWorld>)
{
    auto view = ecsRegistry.view<Transform>();
    for (auto entity : view)
    {
        auto& trans = view.get<Transform>(entity);
        if (trans.GetParent() == entt::null)
        {
            UpdateTransformHierarchy(entity, glm::mat4(1.0f), false);
        }
    }
}
void Pitaya::Game::Scene::UpdateTransformHierarchy(entt::entity entity, const glm::mat4& parentWorldMatrix, bool parentChanged)
{
    auto* trans = ecsRegistry.try_get<Transform>(entity);
    if (!trans) { return; }

    //只有当该节点的局部被修改或者其父节点发生变化时才重新计算乘法
    bool needsUpdate = trans->GetWorldDirty() || parentChanged;
    if (needsUpdate)
    {
        //计算新的世界矩阵 父世界矩阵 * 局部矩阵
        const glm::mat4& localMat = trans->GetLocalMatrix();
        trans->SetWorldMatrix(parentWorldMatrix * localMat);
        trans->SetWorldDirty(false);
    }

    //遍历所有子节点 把当前节点刚刚算出来的 worldMatrix 作为参数传给子节点
    entt::entity currChild = trans->GetFirstChild();
    while (currChild != entt::null)
    {
        entt::entity nextChild = ecsRegistry.get<Transform>(currChild).GetNextSibling();
        //如果当前节点更新了needsUpdate = true 那么所有子节点必然强制更新
        UpdateTransformHierarchy(currChild, trans->GetWorldMatrix(), needsUpdate);
        currChild = nextChild;
    }
}

void Pitaya::Game::Scene::ActionOnGet(GameObject* gameobject)
{
    if (!gameobject) { return; }
    gameobject->Reset();
}
void Pitaya::Game::Scene::ActionOnRelease(GameObject* gameobject)
{
    if (!gameobject) { return; }
    gameobject->Reset();
}