#include<Editor/GUI/Panel/HierarchyPanel.h>
#include<Editor/Editor.h>

void Pitaya::Editor::HierarchyPanel::OnImGuiRender()
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
		auto& selection = Pitaya::Editor::Editor::Instance().GetGUI().GetContext().Selection;
        if (selection.SelectedEntity != entt::null)
        {
            selection.SelectedEntity = entt::null;
			selection.Type = Pitaya::Editor::GUI::Context::Selection::Type::Entity;
        }
    }

    // 右键弹出创建菜单
    DrawEmptyPopup();
}
void Pitaya::Editor::HierarchyPanel::DrawSceneTree()
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
void Pitaya::Editor::HierarchyPanel::DrawEntityNode(Pitaya::Game::Scene* scene, entt::entity entity)
{
    // 获取基础组件信息
    auto* metadata = scene->GetComponent<Pitaya::Game::Metadata>(entity);
    auto* link = scene->GetComponent<Pitaya::Game::ChildLink>(entity);
    const char* name = metadata ? metadata->GetName().data() : "[Error] UnMetadata";
    bool hasChildren = link && link->GetFirstChild() != entt::null;

    // 活性状态视觉处理
    bool isSelfActive = metadata ? metadata->IsActive() : true;
    bool isHierarchyActive = !scene->HasComponent<Pitaya::Game::Disabled>(entity);

    bool pushedColor = false;
    if (!isSelfActive)
    {
        // 自身被禁用：深灰色
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
        pushedColor = true;
    }
    else if (!isHierarchyActive)
    {
        // 自身开着，但父级是关着的 (层级禁用)：半透明淡灰色
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 0.4f));
        pushedColor = true;
    }

    // 配置 TreeNode 标志位
    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth |
        ImGuiTreeNodeFlags_OpenOnDoubleClick;

    auto& selection = Pitaya::Editor::Editor::Instance().GetGUI().GetContext().Selection;
    if (!hasChildren) { flags |= ImGuiTreeNodeFlags_Leaf; }
    if (selection.SelectedEntity == entity) { flags |= ImGuiTreeNodeFlags_Selected; }

    // 绘制节点主体
    bool opened = ImGui::TreeNodeEx((void*)(intptr_t)entity, flags, "%s", name);

    // 绘制完 Label 立即还原颜色，避免颜色污染子节点列表
    if (pushedColor) { ImGui::PopStyleColor(); }

    // 拖拽源 (Drag Source
    if (ImGui::BeginDragDropSource())
    {
        ImGui::SetDragDropPayload("HIERARCHY_ENTITY", &entity, sizeof(entt::entity));
        ImGui::Text("Move %s", name);
        ImGui::EndDragDropSource();
    }

    // 拖拽目标 (Drag Target)
    if (ImGui::BeginDragDropTarget())
    {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("HIERARCHY_ENTITY", ImGuiDragDropFlags_AcceptBeforeDelivery))
        {
            entt::entity draggedEntity = *(const entt::entity*)payload->Data;

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

                // A. 顶部区域：插在当前节点之前 (Sibling)
                if (mouseY < itemRectMinY + itemHeight * 0.25f)
                {
                    drawList->AddLine(ImVec2(lineX, itemRectMinY), ImVec2(lineX + lineW, itemRectMinY), ImColor(255, 255, 0), 2.0f);
                    if (payload->IsDelivery())
                    {
                        auto* selfLink = scene->GetComponent<Pitaya::Game::ChildLink>(entity);
                        entt::entity prevSibling = selfLink->GetPreviousSibling();
                        if (draggedEntity != prevSibling) // 防止重复设置
                        {
                            scene->SetRelationship(draggedEntity, parentId, prevSibling);
                        }
                    }
                }
                // B. 底部区域：插在当前节点之后 (Sibling)
                else if (mouseY > itemRectMaxY - itemHeight * 0.25f)
                {
                    drawList->AddLine(ImVec2(lineX, itemRectMaxY), ImVec2(lineX + lineW, itemRectMaxY), ImColor(255, 255, 0), 2.0f);
                    if (payload->IsDelivery())
                    {
                        scene->SetRelationship(draggedEntity, parentId, entity);
                    }
                }
                // C. 中间区域：成为当前节点的子物体
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

    // 双击聚焦
    if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
    {
        if (auto* transform = scene->GetComponent<Pitaya::Game::Transform>(entity))
        {
            Pitaya::Editor::Editor::Instance().GetCamera().Focus(transform->GetWorldPosition());
        }
    }

    // 点击选中
    if (ImGui::IsItemClicked(ImGuiMouseButton_Left) || ImGui::IsItemClicked(ImGuiMouseButton_Right))
    {
        if (selection.SelectedEntity != entity)
        {
            selection.SelectedEntity = entity;
            selection.Type = Pitaya::Editor::GUI::Context::Selection::Type::Entity;
        }
    }

    // 右键上下文菜单
    if (ImGui::BeginPopupContextItem())
    {
        if (ImGui::MenuItem("Delete Entity"))
        {
            if (selection.SelectedEntity == entity)
            {
                selection.SelectedEntity = entt::null;
                selection.Type = Pitaya::Editor::GUI::Context::Selection::Type::Entity;
            }
            scene->DestroyEntity(entity);
        }
        ImGui::EndPopup();
    }

    // 递归渲染子节点
    if (opened)
    {
        if (hasChildren)
        {
            entt::entity currChild = link->GetFirstChild();
            while (currChild != entt::null)
            {
                auto* childLink = scene->GetComponent<Pitaya::Game::ChildLink>(currChild);
                entt::entity nextChild = childLink ? childLink->GetNextSibling() : entt::null;

                // 递归调用
                DrawEntityNode(scene, currChild);

                currChild = nextChild;
            }
        }
        ImGui::TreePop();
    }
}
void Pitaya::Editor::HierarchyPanel::DrawEmptyPopup()
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
                ImGui::Separator();
                if (ImGui::MenuItem("Sphere"))
                {
                    auto e = scene->CreateEntity("Sphere");
                    scene->AddComponent<Pitaya::Game::MeshRenderer>(e).LoadMesh(Pitaya::Asset::Mesh::Sphere);
                }
                ImGui::Separator();
                if (ImGui::MenuItem("Panel"))
                {
                    auto e = scene->CreateEntity("Panel");
                    scene->AddComponent<Pitaya::Game::MeshRenderer>(e).LoadMesh(Pitaya::Asset::Mesh::Panel);
                }

                //TO REMOVE
                /*ImGui::Separator();
                if (ImGui::MenuItem("Backpack"))
                {
                    auto e = scene->CreateEntity("Backpack");
                    scene->AddComponent<Pitaya::Game::MeshRenderer>(e).LoadMesh(Pitaya::Asset::Mesh::Backpack);
                }*/
                //END TO REMOVE
            }
            ImGui::EndMenu();
        }
        ImGui::EndPopup();
    }
}
