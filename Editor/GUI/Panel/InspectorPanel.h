#pragma once

#include<Editor/GUI/Panel/Panel.h>

namespace Pitaya::Editor
{
    class InspectorPanel : public Panel
    {
        friend class Pitaya::Editor::GUI;
    private:
        explicit InspectorPanel(std::string_view name = "Inspector")
            : Panel(name) {}
        ~InspectorPanel() override = default;

    public:
        InspectorPanel(const InspectorPanel&) = delete;
        InspectorPanel& operator=(const InspectorPanel&) = delete;
        InspectorPanel(InspectorPanel&&) = delete;
        InspectorPanel& operator=(InspectorPanel&&) = delete;

    private:
        inline void SetSelectedEntity(int32_t entityID) noexcept
        {
            selectionID = entityID;
        }

    private:
        void OnImGuiRender() override
        {
            if (selectionID == -1)
            {
                ImGui::Text("No entity selected.");
                return;
            }

            // TODO  根据不同对象执行不同逻辑
            // 根据 ID 显示不同内容（模拟）
            ImGui::Text("Entity ID: %d", selectionID);
            ImGui::Separator();

            // 这里因为封装了类，我们可以放一些 static 或者成员变量来缓存输入框状态
            static float pos[3] = { 0.0f, 0.0f, 0.0f };
            ImGui::DragFloat3("Position", pos, 0.1f);

            static float rot[3] = { 0.0f, 0.0f, 0.0f };
            ImGui::SliderFloat3("Rotation", rot, 0.0f, 360.0f);

            if (ImGui::Button("AddComponent"))
            {
                ImGui::OpenPopup("AddComponentPopup");
            }
        }

    private:
        uint32_t selectionID = -1;
    };
}
