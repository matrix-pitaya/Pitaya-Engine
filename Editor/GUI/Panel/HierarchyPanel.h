#pragma once

#include<Editor/GUI/Panel/Panel.h>

#include<functional>

namespace Pitaya::Editor
{
	class HierarchyPanel : public Panel
	{
        friend class  Pitaya::Editor::GUI;
    private:
        explicit HierarchyPanel(std::string_view name = "Hierarchy")
            : Panel(name) {}
        ~HierarchyPanel() override = default;

    public:
        HierarchyPanel(const HierarchyPanel&) = delete;
        HierarchyPanel& operator=(const HierarchyPanel&) = delete;
        HierarchyPanel(HierarchyPanel&&) = delete;
        HierarchyPanel& operator=(HierarchyPanel&&) = delete;

    private:
        void SetSelectionChangedCallback(std::function<void(int32_t)> callback)
        {
            OnSelectionChanged = std::move(callback);
        }

    private:
        void OnImGuiRender() override
        {
            //绘制场景
            DrawSceneTree();
        
            //取消选中
            if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !ImGui::IsAnyItemHovered())
            {
                if (selectedEntityID != -1)
                {
                    selectedEntityID = -1;
                    if (OnSelectionChanged) OnSelectionChanged(selectedEntityID);
                }
            }

            //右键弹出框
            DrawEmptyPopup();
        }

    private:
        inline void DrawSceneTree()
        {
            // TODO 模拟一些游戏物体 实际需要获取Scene指针
            DrawEntityNode(1, "Player");
            DrawEntityNode(2, "Main Camera");
            DrawEntityNode(3, "Cube Map");
            // Ended
        }
        inline void DrawEntityNode(int entityID, const char* name)
        {
            ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth | 
                ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_Leaf;

            if (selectedEntityID == entityID) { flags |= ImGuiTreeNodeFlags_Selected; }

            bool opened = ImGui::TreeNodeEx((void*)(intptr_t)entityID, flags, name);

            //支持左键与右键选中
            if (ImGui::IsItemClicked(ImGuiMouseButton_Left) || ImGui::IsItemClicked(ImGuiMouseButton_Right))
            {
                if (selectedEntityID != entityID) //只有状态改变时才通知
                {
                    selectedEntityID = entityID;
                    if (OnSelectionChanged) OnSelectionChanged(selectedEntityID); //通知外部
                }
            }

            //右键菜单
            if (ImGui::BeginPopupContextItem())
            {
                if (ImGui::MenuItem("Copy"))
                {
                    // TODO: 执行对 [entityID] 复制逻辑
                }

                ImGui::Separator();

                if (ImGui::MenuItem("Delete"))
                {
                    // TODO: 执行对 [entityID] 删除逻辑

                    // 删完后如果选中项就是被删的，清空选中状态
                    if (selectedEntityID == entityID)
                    {
                        selectedEntityID = -1;
                        if (OnSelectionChanged) OnSelectionChanged(-1);
                    }
                }
                ImGui::EndPopup();
            }

            //如果有子节点 则递归绘制
            if (opened) { ImGui::TreePop(); }
        }
        inline void DrawEmptyPopup()
        {
            if (ImGui::BeginPopupContextWindow("PanelEmptySpaceMenu", ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverItems))
            {
                if (ImGui::BeginMenu("Create"))
                {
                    if (ImGui::MenuItem("Empty Entity"))
                    {
                        //TODO 创建空物体
                    }

                    ImGui::Separator();

                    if (ImGui::MenuItem("Cube"))
                    {
                        //TODO 创建正方形
                    }

                    ImGui::Separator();

                    if (ImGui::MenuItem("Sphere"))
                    {
                        //TODO 创建球形
                    }

                    ImGui::EndMenu();
                }
                ImGui::EndPopup();
            }
        }

    private:
        int32_t selectedEntityID = -1;
        std::function<void(int32_t)> OnSelectionChanged;
	};
}
