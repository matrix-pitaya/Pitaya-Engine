#pragma once

#include<Core/Allocate/Allocate.h>
#include<Core/PassKey/PassKey.h>
#include<Game/Component/Metadata.h>
#include<Game/Component/Transform.h>
#include<Game/Component/Parent.h>
#include<Game/Component/ChildLink .h>
#include<Game/Component/Disabled.h>

#include<entt/entt.hpp>
#include<glm.hpp>

namespace Pitaya::Game
{
    class GameWorld;
    class Scene
    {
        friend class GameWorld;
    private:
        Scene() = default;
        ~Scene() = default;

    public:
        Scene(const Scene&) = delete;
        Scene& operator=(const Scene&) = delete;
        Scene(Scene&&) = delete;
        Scene& operator=(Scene&&) = delete;

    public:
        inline entt::entity GetRootEntity() const noexcept
        {
            return root;
        }
        inline entt::entity CreateEntity(std::string_view name = "GameObject")
        {
            entt::entity entity = ecsRegistry.create();
            AddComponent<Metadata>(entity, name);
            AddComponent<Transform>(entity);
            if (root == entt::null)
            {
                root = entity;
                tail = entity;
                AddComponent<ChildLink>(entity);
            }
            else
            {
                SetRelationship(entity, entt::null, tail);
            }
            return entity;
        }
        inline void DestroyEntity(entt::entity entity)
        {
            if (!ecsRegistry.valid(entity)) { return; }
            delayDestroyQueue.push_back(entity);
        }
        inline void SetRelationship(entt::entity childId, entt::entity parentId, entt::entity afterSibling = entt::null, bool keepWorldTransform = true)
        {
            if (childId == parentId || !ecsRegistry.valid(childId)) { return; }
            if (parentId != entt::null && IsDescendant(parentId, childId)) { return; }

            // 记录当前的世界矩阵 用于保持世界坐标不变
            glm::mat4 currentWorldMatrix = glm::mat4(1.0f);
            auto* childTransform = GetComponent<Transform>(childId);
            if (childTransform && keepWorldTransform)
            {
                currentWorldMatrix = childTransform->GetWorldMatrix();
            }

            UnlinkFromCurrent(childId);

            // 建立新链接
            if (parentId != entt::null)
            {
                AddComponent<Parent>(childId).SetId(parentId);
                if (!HasComponent<ChildLink>(parentId)) { AddComponent<ChildLink>(parentId); }
                InsertIntoLinkList(childId, parentId, afterSibling);
            }
            else
            {
                RemoveComponent<Parent>(childId);
                InsertIntoRootList(childId, afterSibling);
            }

            // 重算 Transform 属性
            if (childTransform)
            {
                if (keepWorldTransform)
                {
                    glm::mat4 parentWorldMatrix = glm::mat4(1.0f);
                    if (parentId != entt::null)
                    {
                        if (auto* parentTransform = GetComponent<Transform>(parentId))
                        {
                            parentWorldMatrix = parentTransform->GetWorldMatrix();
                        }
                    }
                    glm::mat4 newLocalMatrix = glm::inverse(parentWorldMatrix) * currentWorldMatrix;
                    childTransform->SetLocalFromMatrix(newLocalMatrix);
                }

                // 层级变化标记脏逻辑
                childTransform->MarkDirty();
            }

            // 改变层级时立刻标记 Metadata 为脏 强制 UpdateHierarchy 在下一帧重刷它的 Active 状态
            if (auto* metadata = GetComponent<Metadata>(childId))
            {
                metadata->MarkDirty(Pitaya::Core::PassKey<Scene>());
            }
        }

    public:
        template<typename... Owned, typename... Get>
        inline auto GetGroup(entt::get_t<Get...> get)
        {
            return ecsRegistry.group<Owned...>(get);
        }
        template<typename... Owned, typename... Get, typename... Exclude>
        inline auto GetGroup(entt::get_t<Get...> get, entt::exclude_t<Exclude...> exclude)
        {
            return ecsRegistry.group<Owned...>(get, exclude);
        }
        template<typename... Components>
        inline auto GetView()
        {
            return ecsRegistry.view<Components...>();
        }
        template<typename... Components, typename... Excludables>
        inline auto GetView(entt::exclude_t<Excludables...> exclude)
        {
            return ecsRegistry.view<Components...>(exclude);
        }
        template<typename Func>
        inline void Visit(entt::entity entity, Func&& func) const
        {
            if (!ecsRegistry.valid(entity)) { return; }
            for (auto&& [id, storage] : ecsRegistry.storage())
            {
                if (storage.contains(entity)) { func(id); }
            }
        }
        template<typename T, typename... Args>
        inline T& AddComponent(entt::entity entity, Args&&... args)
        {
            return ecsRegistry.emplace_or_replace<T>(entity, std::forward<Args>(args)...);
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
            static_assert(!std::is_same_v<T, Metadata> || !std::is_same_v<T, Transform>,
                "Can't Remove Metadata or Transform Component");
            ecsRegistry.remove<T>(entity);
        }

    private:
        inline void ProcessDelayDestroyQueue()
        {
            for (size_t i = 0; i < delayDestroyQueue.size(); ++i)
            {
                entt::entity entity = delayDestroyQueue[i];
                if (!ecsRegistry.valid(entity)) { continue; }

                UnlinkFromCurrent(entity);

                if (auto* link = GetComponent<ChildLink>(entity))
                {
                    entt::entity currChild = link->GetFirstChild();
                    while (currChild != entt::null)
                    {
                        entt::entity nextChild = GetComponent<ChildLink>(currChild)->GetNextSibling();
                        DestroyEntity(currChild);
                        currChild = nextChild;
                    }
                }
                ecsRegistry.destroy(entity);
            }
            delayDestroyQueue.clear();
        }
        inline void ProcessHierarchySystem()
        {
            auto rootCandidates = ecsRegistry.view<Transform>(entt::exclude<Parent>);
            for (auto entity : rootCandidates)
            {
                UpdateHierarchy(entity, glm::mat4(1.0f), false, true, false);
            }
        }

    private:
        inline void UpdateHierarchy(entt::entity entity, const glm::mat4& parentWorldMatrix, bool parentChanged, bool parentActive, bool parentActiveChanged)
        {
            // 处理 Transform 系统
            auto* trans = ecsRegistry.try_get<Transform>(entity);
            bool transformNeedsUpdate = false;
            glm::mat4 currWorldMatrix = parentWorldMatrix;
            if (trans)
            {
                transformNeedsUpdate = trans->IsDirty() || parentChanged;
                if (transformNeedsUpdate)
                {
                    currWorldMatrix = parentWorldMatrix * trans->GetLocalMatrix();
                    trans->SetWorldMatrix(currWorldMatrix); // 内部执行了 ClearDirty
                }
                else
                {
                    currWorldMatrix = trans->GetWorldMatrix();
                }
            }

            // 处理 Active 系统
            auto* metadata = ecsRegistry.try_get<Metadata>(entity);
            bool selfActive = true;
            bool activeDirty = false;
            if (metadata)
            {
                selfActive = metadata->IsActive();
                activeDirty = metadata->IsDirty();
            }

            bool isHierarchyActive = parentActive && selfActive;
            bool activeNeedsUpdate = parentActiveChanged || activeDirty;
            if (activeNeedsUpdate)
            {
                bool isCurrentlyDisabled = ecsRegistry.all_of<Disabled>(entity);
                if (isHierarchyActive)
                {
                    if (isCurrentlyDisabled) 
                    { 
                        ecsRegistry.remove<Disabled>(entity);
                    }
                }
                else
                {
                    if (!isCurrentlyDisabled) 
                    { 
                        ecsRegistry.emplace_or_replace<Disabled>(entity);
                    }
                }

                if (metadata && activeDirty) { metadata->MarkClear(Pitaya::Core::PassKey<Scene>()); }
            }

            // 递归处理子节点
            if (auto* link = ecsRegistry.try_get<ChildLink>(entity))
            {
                entt::entity currChild = link->GetFirstChild();
                while (currChild != entt::null)
                {
                    // 获取下一个兄弟节点缓存，防止子节点销毁影响遍历
                    entt::entity nextChild = ecsRegistry.get<ChildLink>(currChild).GetNextSibling();

                    // 下传 parentDirty(transformNeedsUpdate) 和 activeDirty(activeNeedsUpdate)
                    UpdateHierarchy(currChild, currWorldMatrix, transformNeedsUpdate, isHierarchyActive, activeNeedsUpdate);

                    currChild = nextChild;
                }
            }
        }
        inline bool IsDescendant(entt::entity maybeChild, entt::entity maybeParent)
        {
            if (maybeChild == maybeParent) { return true; }
            if (!ecsRegistry.valid(maybeChild)) { return false; }
            entt::entity current = maybeChild;
            while (auto* parentComp = GetComponent<Parent>(current))
            {
                if (parentComp->GetId() == maybeParent) { return true; }
                current = parentComp->GetId();
            }
            return false;
        }
        inline void UnlinkFromCurrent(entt::entity entity)
        {
            auto* link = GetComponent<ChildLink>(entity);
            if (!link) { return; }

            entt::entity prev = link->GetPreviousSibling();
            entt::entity next = link->GetNextSibling();
            entt::entity parentId = HasComponent<Parent>(entity) ? GetComponent<Parent>(entity)->GetId() : entt::null;

            if (prev != entt::null) { GetComponent<ChildLink>(prev)->SetNextSibling(next); }
            else if (parentId != entt::null) { GetComponent<ChildLink>(parentId)->SetFirstChild(next); }
            else if (entity == root) { root = next; }

            if (next != entt::null) { GetComponent<ChildLink>(next)->SetPreviousSibling(prev); }
            else if (parentId == entt::null && entity == tail) { tail = prev; }

            link->SetNextSibling(entt::null);
            link->SetPreviousSibling(entt::null);
        }
        inline void InsertIntoLinkList(entt::entity child, entt::entity parent, entt::entity after)
        {
            if (!HasComponent<ChildLink>(child)) { AddComponent<ChildLink>(child); }
            auto* pLink = GetComponent<ChildLink>(parent);
            auto* cLink = GetComponent<ChildLink>(child);

            if (after == entt::null)
            {
                entt::entity oldFirst = pLink->GetFirstChild();
                cLink->SetNextSibling(oldFirst);
                if (oldFirst != entt::null) { GetComponent<ChildLink>(oldFirst)->SetPreviousSibling(child); }
                pLink->SetFirstChild(child);
            }
            else
            {
                auto* aLink = GetComponent<ChildLink>(after);
                entt::entity next = aLink->GetNextSibling();
                aLink->SetNextSibling(child);
                cLink->SetPreviousSibling(after);
                cLink->SetNextSibling(next);
                if (next != entt::null) { GetComponent<ChildLink>(next)->SetPreviousSibling(child); }
            }
        }
        inline void InsertIntoRootList(entt::entity child, entt::entity after)
        {
            if (!HasComponent<ChildLink>(child)) { AddComponent<ChildLink>(child); }
            auto* cLink = GetComponent<ChildLink>(child);
            if (after == entt::null || root == entt::null)
            {
                cLink->SetNextSibling(root);
                if (root != entt::null) { GetComponent<ChildLink>(root)->SetPreviousSibling(child); }
                root = child;
                if (tail == entt::null) { tail = child; }
            }
            else
            {
                auto* aLink = GetComponent<ChildLink>(after);
                entt::entity next = aLink->GetNextSibling();
                aLink->SetNextSibling(child);
                cLink->SetPreviousSibling(after);
                cLink->SetNextSibling(next);
                if (next != entt::null) { GetComponent<ChildLink>(next)->SetPreviousSibling(child); }
                else { tail = child; }
            }
        }

    private:
        entt::entity root = entt::null;
        entt::entity tail = entt::null;
        entt::registry ecsRegistry;
        std::vector<entt::entity> delayDestroyQueue;
    };
}