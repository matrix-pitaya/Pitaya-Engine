#pragma once

#include<Editor/GUI/Panel/Panel.h>
#include<Game/Common/FuncTable.h>
#include<Game/Scene/Scene.h>
#include<Game/Component/Camera.h>
#include<Game/Component/Tag.h>
#include<Game/Component/Parent.h>
#include<Game/Component/MeshRenderer.h>
#include<Asset/Common/Mesh.h>

#include<functional>

namespace Pitaya::Editor
{
    class HierarchyPanel : public Panel
    {
        friend class Pitaya::Editor::GUI;

    private:
        explicit HierarchyPanel(std::string_view name = "Hierarchy")
            : Panel(name) { }
        ~HierarchyPanel() override = default;

    public:
        HierarchyPanel(const HierarchyPanel&) = delete;
        HierarchyPanel& operator=(const HierarchyPanel&) = delete;

    private:
        void SetSelectionChangedCallback(std::function<void(entt::entity)> callback) noexcept
        {
            OnSelectionChanged = std::move(callback);
        }

    private:
        void OnImGuiRender() override
        {
            // 绘制场景树主体
            DrawSceneTree();

            // 拖拽到面板最下方空白处：变回根节点末尾
            ImVec2 availSpace = ImGui::GetContentRegionAvail();
            ImGui::Dummy(ImVec2(availSpace.x, std::max(availSpace.y, 40.0f)));

            if (ImGui::BeginDragDropTarget())
            {
                if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("HIERARCHY_ENTITY"))
                {
                    entt::entity draggedEntity = *(const entt::entity*)payload->Data;
                    if (auto* scene = Pitaya::Game::GetActiveScene())
                    {
                        // 找到当前链表中最后一个根节点
                        entt::entity lastRoot = entt::null;
                        entt::entity curr = scene->GetRootEntity();
                        while (curr != entt::null)
                        {
                            lastRoot = curr;
                            auto* link = scene->GetComponent<Pitaya::Game::ChildLink>(curr);
                            curr = link ? link->GetNextSibling() : entt::null;
                        }

                        // 如果要拖拽的节点本身就是最后一个根节点，则无需操作
                        if (draggedEntity != lastRoot)
                        {
                            scene->SetRelationship(draggedEntity, entt::null, lastRoot);
                        }
                    }
                }
                ImGui::EndDragDropTarget();
            }

            // 点击空白区域取消选中逻辑
            if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !ImGui::IsAnyItemHovered())
            {
                if (selectedEntity != entt::null)
                {
                    selectedEntity = entt::null;
                    if (OnSelectionChanged) { OnSelectionChanged(selectedEntity); }
                }
            }

            // 右键弹出创建菜单
            DrawEmptyPopup();
        }

    private:
        inline void DrawSceneTree()
        {
            if (auto* scene = Pitaya::Game::GetActiveScene())
            {
                entt::entity curr = scene->GetRootEntity();
                while (curr != entt::null)
                {
                    // 注意：在循环开始前获取下一个节点，防止操作改变了当前节点的 Next 指针
                    auto* link = scene->GetComponent<Pitaya::Game::ChildLink>(curr);
                    entt::entity next = link ? link->GetNextSibling() : entt::null;

                    DrawEntityNode(scene, curr);

                    curr = next;
                }
            }
        }
        inline void DrawEntityNode(Pitaya::Game::Scene* scene, entt::entity entity)
        {
            auto* tag = scene->GetComponent<Pitaya::Game::Tag>(entity);
            auto* link = scene->GetComponent<Pitaya::Game::ChildLink>(entity);
            const char* name = tag ? tag->GetName().data() : "[Error] UnTag";
            bool hasChildren = link && link->GetFirstChild() != entt::null;

            ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth |
                ImGuiTreeNodeFlags_OpenOnDoubleClick;

            if (!hasChildren) { flags |= ImGuiTreeNodeFlags_Leaf; }
            if (selectedEntity == entity) { flags |= ImGuiTreeNodeFlags_Selected; }

            bool opened = ImGui::TreeNodeEx((void*)(intptr_t)entity, flags, "%s", name);

            // 拖拽源处理 
            if (ImGui::BeginDragDropSource())
            {
                ImGui::SetDragDropPayload("HIERARCHY_ENTITY", &entity, sizeof(entt::entity));
                ImGui::Text("Move %s", name);
                ImGui::EndDragDropSource();
            }

            // 拖拽目标与排序逻辑
            if (ImGui::BeginDragDropTarget())
            {
                // 使用 AcceptBeforeDelivery 允许我们在悬停时画辅助线
                if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("HIERARCHY_ENTITY", ImGuiDragDropFlags_AcceptBeforeDelivery))
                {
                    entt::entity draggedEntity = *(const entt::entity*)payload->Data;

                    // 不能把自己拖给自己，也不能把父节点拖给子节点 (底层 IsDescendant 已保护)
                    if (draggedEntity != entity)
                    {
                        float mouseY = ImGui::GetMousePos().y;
                        float itemRectMinY = ImGui::GetItemRectMin().y;
                        float itemRectMaxY = ImGui::GetItemRectMax().y;
                        float itemHeight = itemRectMaxY - itemRectMinY;

                        auto* parentComp = scene->GetComponent<Pitaya::Game::Parent>(entity);
                        entt::entity parentId = parentComp ? parentComp->GetId() : entt::null;

                        ImDrawList* drawList = ImGui::GetWindowDrawList();
                        float lineX = ImGui::GetItemRectMin().x;
                        float lineW = ImGui::GetContentRegionAvail().x;

                        // A. 顶部区域 (上 25%) -> 插在 entity 之前
                        if (mouseY < itemRectMinY + itemHeight * 0.25f)
                        {
                            drawList->AddLine(ImVec2(lineX, itemRectMinY), ImVec2(lineX + lineW, itemRectMinY), ImColor(255, 255, 0), 2.0f);
                            if (payload->IsDelivery())
                            {
                                auto* selfLink = scene->GetComponent<Pitaya::Game::ChildLink>(entity);
                                entt::entity prevSibling = selfLink->GetPreviousSibling();
                                if (draggedEntity != prevSibling && draggedEntity != entity)    // 防止自己插在自己前面导致消失
                                {
                                    scene->SetRelationship(draggedEntity, parentId, prevSibling);
                                }
                            }
                        }
                        // B. 底部区域 (下 25%) -> 插在 entity 之后
                        else if (mouseY > itemRectMaxY - itemHeight * 0.25f)
                        {
                            drawList->AddLine(ImVec2(lineX, itemRectMaxY), ImVec2(lineX + lineW, itemRectMaxY), ImColor(255, 255, 0), 2.0f);
                            if (payload->IsDelivery())
                            {
                                // 防止自己插在自己后面
                                if (draggedEntity != entity)
                                {
                                    scene->SetRelationship(draggedEntity, parentId, entity);
                                }
                            }
                        }
                        // C. 中间区域 -> 成为其子物体
                        else
                        {
                            if (payload->IsDelivery())
                            {
                                scene->SetRelationship(draggedEntity, entity, entt::null);
                            }
                        }
                    }
                }
                ImGui::EndDragDropTarget();
            }

            // 鼠标选中逻辑 
            if (ImGui::IsItemClicked(ImGuiMouseButton_Left) || ImGui::IsItemClicked(ImGuiMouseButton_Right))
            {
                if (selectedEntity != entity)
                {
                    selectedEntity = entity;
                    if (OnSelectionChanged) { OnSelectionChanged(selectedEntity); }
                }
            }

            // 右键删除菜单 
            if (ImGui::BeginPopupContextItem())
            {
                if (ImGui::MenuItem("Delete Entity"))
                {
                    // 如果要删除的是当前选中的实体，清空指针防止 UI 崩溃
                    if (selectedEntity == entity)
                    {
                        selectedEntity = entt::null;
                        if (OnSelectionChanged) { OnSelectionChanged(entt::null); }
                    }
                    scene->DestroyEntity(entity);
                }
                ImGui::EndPopup();
            }

            // 递归子节点渲染
            if (opened)
            {
                if (hasChildren)
                {
                    entt::entity currChild = link->GetFirstChild();
                    while (currChild != entt::null)
                    {
                        // 同样在循环内获取 Next，防止 DrawEntityNode 操作导致链表变化
                        auto* childLink = scene->GetComponent<Pitaya::Game::ChildLink>(currChild);
                        entt::entity nextChild = childLink ? childLink->GetNextSibling() : entt::null;

                        DrawEntityNode(scene, currChild);

                        currChild = nextChild;
                    }
                }
                ImGui::TreePop();
            }
        }
        inline void DrawEmptyPopup()
        {
            if (ImGui::BeginPopupContextWindow("PanelEmptySpaceMenu", ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverItems))
            {
                if (ImGui::BeginMenu("Create"))
                {
                    auto* scene = Pitaya::Game::GetActiveScene();
                    if (scene)
                    {
                        if (ImGui::MenuItem("Empty")) { scene->CreateEntity("Empty"); }
                        ImGui::Separator();
                        if (ImGui::MenuItem("Cube")) 
                        {
                            auto e = scene->CreateEntity("Cube");
                            scene->AddComponent<Pitaya::Game::MeshRenderer>(e).LoadMesh(Pitaya::Asset::Mesh::Cube);
                        }
                        if (ImGui::MenuItem("Backpack")) 
                        {
                            auto e = scene->CreateEntity("Backpack");
                            scene->AddComponent<Pitaya::Game::MeshRenderer>(e).LoadMesh(Pitaya::Asset::Mesh::Backpack);
                        }
                        if (ImGui::MenuItem("Panel")) 
                        {
                            auto e = scene->CreateEntity("Panel");
                            scene->AddComponent<Pitaya::Game::MeshRenderer>(e).LoadMesh(Pitaya::Asset::Mesh::Panel);
                        }
                        if (ImGui::MenuItem("Planet")) 
                        {
                            auto e = scene->CreateEntity("Planet");
                            scene->AddComponent<Pitaya::Game::MeshRenderer>(e).LoadMesh(Pitaya::Asset::Mesh::Planet);
                        }
                        if (ImGui::MenuItem("Rock")) 
                        {
                            auto e = scene->CreateEntity("Rock");
                            scene->AddComponent<Pitaya::Game::MeshRenderer>(e).LoadMesh(Pitaya::Asset::Mesh::Rock);
                        }
                    }
                    ImGui::EndMenu();
                }
                ImGui::EndPopup();
            }
        }

    private:
        entt::entity selectedEntity = entt::null;
        std::function<void(entt::entity)> OnSelectionChanged;
    };
}
