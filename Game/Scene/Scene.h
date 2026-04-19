#pragma once

#include<Core/Allocate/Allocate.h>
#include<Core/PassKey/PassKey.h>
#include<Core/Container/ObjectPool.h>
#include<Game/Entity/GameObject.h>
#include<Game/Component/Tag.h>
#include<Game/Component/Parent.h>
#include<Game/Component/ChildLink .h>
#include<Game/Component/Transform.h>

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
        inline entt::entity CreateEntity(std::string_view name = "GameObject", std::string_view tag = "default")
        {
            entt::entity entity = ecsRegistry.create();
            AddComponent<Tag>(entity, name, tag);
            AddComponent<Transform>(entity);
            if (root == entt::null) 
            {
                root = entity;
                AddComponent<ChildLink>(entity);
            }
            else 
            {
                // 找到最后一个根节点并链接
                entt::entity curr = root;
                while (GetComponent<ChildLink>(curr)->GetNextSibling() != entt::null) 
                {
                    curr = GetComponent<ChildLink>(curr)->GetNextSibling();
                }
                SetRelationship(entity, entt::null, curr); // 传 curr 表示在它之后
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

            // 在断开连接前，记录当前的世界矩阵
            glm::mat4 currentWorldMatrix = glm::mat4(1.0f);
            auto* childTransform = GetComponent<Transform>(childId);
            if (childTransform && keepWorldTransform)
            {
                currentWorldMatrix = childTransform->GetWorldMatrix();
            }

            UnlinkFromCurrent(childId);

            if (parentId != entt::null)
            {
                AddComponent<Parent>(childId).SetId(parentId);
                if (!HasComponent<ChildLink>(parentId)) { AddComponent<ChildLink>(parentId); }
                InsertIntoLinkList(childId, parentId, afterSibling);
            }
            else
            {
                // 处理为根节点
                RemoveComponent<Parent>(childId);
                InsertIntoRootList(childId, afterSibling);
            }

            // 链接完成后，根据新父节点的世界矩阵，反推新的 Local 属性
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

                    // 新的局部矩阵 = 逆(新父节点的世界矩阵) * 原始世界矩阵
                    glm::mat4 newLocalMatrix = glm::inverse(parentWorldMatrix) * currentWorldMatrix;

                    // 使用你已有的 SetLocalFromMatrix 来分解更新 localPosition/Rotation/Scale
                    childTransform->SetLocalFromMatrix(newLocalMatrix);
                }

                // 无论何种情况，层级变化必须强制标记脏数据
                childTransform->MarkDirty();
            }
        }

    public:
        // 注：Group适用于组件数量非常多的情况但需要注意同一个主键的entt::get必须保持一致
        // 渲染器MeshRenderer[Transform]、刚体RigidBody[Transform]、粒子Particle[Transform]、2D精灵图Sprite[Transform]
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
            ecsRegistry.remove<T>(entity);
        }


    private:
		inline void ProcessDelayDestroyQueue()  // 在 EndFrame 调用
        {
            // 注：此处必须通过下标索引循环 因为在循环体内调用 DestroyEntity 会导致 queue.size() 动态增长
            for (size_t i = 0; i < delayDestroyQueue.size(); ++i)
            {
                entt::entity entity = delayDestroyQueue[i];

                // 防止同一个实体在队列中出现多次导致的重复销毁崩溃
                if (!ecsRegistry.valid(entity)) { continue; }

                // 仅仅脱离当前链表指针 不要使用 SetRelationship 以免又被加进 root 链表
                UnlinkFromCurrent(entity);

                // 层级销毁 通过 ChildLink 安全遍历子节点
                if (auto* link = GetComponent<ChildLink>(entity))
                {
                    entt::entity currChild = link->GetFirstChild();
                    while (currChild != entt::null)
                    {
                        entt::entity nextChild = GetComponent<ChildLink>(currChild)->GetNextSibling();

                        // 立即切断子节点与当前父节点的联系
                        // 防止子节点出队执行 UnlinkFromCurrent 时 尝试访问已被彻底销毁的父节点而崩溃
                        RemoveComponent<Parent>(currChild);

                        // 然后将子节点加入延迟销毁队列
                        DestroyEntity(currChild);
                        currChild = nextChild;
                    }
                }

                // 在 ECS 注册表中彻底抹除此实体
                ecsRegistry.destroy(entity);
            }

            // 队列处理完毕 统一清空
            delayDestroyQueue.clear();
        }
		inline void ProcessTransformSystem()    // 在 LateUpdate 调用
        {
            auto rootCandidates = ecsRegistry.view<Transform>(entt::exclude<Parent>);
            for (auto entity : rootCandidates)
            {
                UpdateTransformHierarchy(entity, glm::mat4(1.0f), false);
            }
        }

    private:
        inline void UpdateTransformHierarchy(entt::entity entity, const glm::mat4& parentWorldMatrix, bool parentChanged)
        {
            auto* trans = ecsRegistry.try_get<Transform>(entity);
            if (!trans) { return; }

            bool needsUpdate = trans->IsDirty() || parentChanged;
            glm::mat4 currWorldMatrix = trans->GetWorldMatrix();

            if (needsUpdate)
            {
                const glm::mat4& localMat = trans->GetLocalMatrix();
                currWorldMatrix = parentWorldMatrix * localMat;
                trans->SetWorldMatrix(currWorldMatrix);
            }

            // 只对含有 ChildLink 且拥有子节点的实体发起递归查询
            if (auto* link = ecsRegistry.try_get<ChildLink>(entity))
            {
                entt::entity currChild = link->GetFirstChild();
                while (currChild != entt::null)
                {
                    entt::entity nextChild = ecsRegistry.get<ChildLink>(currChild).GetNextSibling();
                    UpdateTransformHierarchy(currChild, currWorldMatrix, needsUpdate);
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
            else if (entity == root) { root = next; } // 如果是根节点的头节点

            if (next != entt::null) { GetComponent<ChildLink>(next)->SetPreviousSibling(prev); }

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

	private:
		entt::entity root = entt::null; // 供UI显示使用的根节点链表头 通过 ChildLink 连接所有根节点
		entt::registry ecsRegistry;
		std::vector<entt::entity> delayDestroyQueue;
	};
}
