#include<Editor/GUI/Panel/InspectorPanel.h>
#include<Editor/Editor.h>

void Pitaya::Editor::InspectorPanel::OnImGuiRender()
{
    switch (Pitaya::Editor::Editor::Instance().GetGUI().GetContext().Selection.Type)
    {
        case Pitaya::Editor::GUI::Context::Selection::Type::Entity:  DrawEntity(); break;
        case Pitaya::Editor::GUI::Context::Selection::Type::File:    DrawFile();   break;
    }
}
void Pitaya::Editor::InspectorPanel::DrawEntity()
{
    entt::entity entity = Pitaya::Editor::Editor::Instance().GetGUI().GetContext().Selection.SelectedEntity;
    if (entity == entt::null) { return; }

    if (auto* scene = Pitaya::Game::GetActiveScene())
    {
        DrawTag(scene->GetComponent<Pitaya::Game::Tag>(entity));
        DrawTransform(scene->GetComponent<Pitaya::Game::Transform>(entity));
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    if (ImGui::Button("Add Component", ImVec2(-1, 0)))
    {
        // 在打开弹窗前 计算并设置弹窗位置 按钮正下方 左对齐
        ImVec2 buttonMin = ImGui::GetItemRectMin();
        ImVec2 buttonSize = ImGui::GetItemRectSize();
        ImGui::SetNextWindowPos(ImVec2(buttonMin.x, buttonMin.y + buttonSize.y));
        ImGui::SetNextWindowSize(ImVec2(buttonSize.x, 0)); // 宽度和按钮完全一致 高度自动

        ImGui::OpenPopup("AddComponentPopup");
    }

    if (ImGui::BeginPopup("AddComponentPopup", ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize))
    {
        const constexpr uint32_t MAX_SEARCH_SIZE = 256;

        // 顶部搜索框
        static char InputBuffer[MAX_SEARCH_SIZE] = {};
        ImGui::PushItemWidth(-1);
        ImGui::InputTextWithHint("##Search", "Search...", InputBuffer, sizeof(InputBuffer));
        ImGui::PopItemWidth();
        ImGui::Spacing();

        // 组件列表
        struct ComponentItem
        {
            const char* Name = "";
            const char* Category = "";
        };
        ComponentItem components[] = {
            {"Mesh Renderer",       "Render"},
            {"Camera",              "Render"},
            {"Rigidbody",           "Physics"},
            {"Box Collider",        "Physics"},
            {"Sphere Collider",     "Physics"},
            {"Audio Source",        "Audio"},
            {"C# Script",           "Scripts"},
        };

        // 遍历并过滤
        char inputLower[MAX_SEARCH_SIZE] = {};
        Pitaya::Core::CopyStringToCharArray(InputBuffer, inputLower);
        Pitaya::Core::ToLower(inputLower);

        ImGui::BeginChild("ComponentList", ImVec2(0, 150), true);
        for (auto& item : components)
        {
            char nameLower[MAX_SEARCH_SIZE] = {};
            Pitaya::Core::CopyStringToCharArray(item.Name, nameLower);
            Pitaya::Core::ToLower(nameLower);

            if (inputLower[0] == '\0' || std::strstr(nameLower, inputLower) != nullptr)
            {
                if (ImGui::Selectable(item.Name))
                {
                    // TODO: 添加组件逻辑


                    // 清空输入框
                    InputBuffer[0] = '\0';
                    ImGui::CloseCurrentPopup();
                }
            }
        }
        ImGui::EndChild();

        ImGui::EndPopup();
    }
}
void Pitaya::Editor::InspectorPanel::DrawFile()
{
    //TODO 显示文件信息
}
void Pitaya::Editor::InspectorPanel::DrawTag(Pitaya::Game::Tag* tag)
{
    if (!tag || !ImGui::CollapsingHeader("Tag", ImGuiTreeNodeFlags_DefaultOpen)) { return; }

    ImGui::Indent(16.0f);

    constexpr float LABEL_WIDTH = 70.0f;
    constexpr float RIGHT_PADDING = 8.0f;

    {   // Name
        char buffer[Game::Tag::MAX_STR_SIZE] = {};
        Pitaya::Core::CopyStringToCharArray(tag->GetName(), buffer);

        ImGui::AlignTextToFramePadding();
        ImGui::TextUnformatted("Name");
        ImGui::SameLine(LABEL_WIDTH);

        ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x - RIGHT_PADDING);
        if (ImGui::InputText("##Name_TagComponent", buffer, Game::Tag::MAX_STR_SIZE))
        {
            tag->SetName(buffer);
        }
        ImGui::PopItemWidth();
    }

    {   // Tag
        char buffer[Game::Tag::MAX_STR_SIZE] = {};
        Pitaya::Core::CopyStringToCharArray(tag->GetTag(), buffer);

        ImGui::AlignTextToFramePadding();
        ImGui::TextUnformatted("Tag");
        ImGui::SameLine(LABEL_WIDTH);

        ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x - RIGHT_PADDING);
        if (ImGui::InputText("##Tag_TagComponent", buffer, Game::Tag::MAX_STR_SIZE))
        {
            tag->SetTag(buffer);
        }
        ImGui::PopItemWidth();
    }

    ImGui::Unindent(16.0f);
}
void Pitaya::Editor::InspectorPanel::DrawTransform(Pitaya::Game::Transform* transform)
{
    if (!transform || !ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen)) { return; }

    ImGui::Indent(16.0f);

    constexpr const float LABEL_WIDTH = 70.0f;
    float availWidth = ImGui::GetContentRegionAvail().x;
    float fieldWidth = (availWidth - LABEL_WIDTH - 8.0f) / 3.0f;

    {   // Position 
        glm::vec3 pos = transform->GetLocalPosition();
        ImGui::AlignTextToFramePadding();
        ImGui::TextUnformatted("Position");
        ImGui::SameLine(LABEL_WIDTH);

        ImGui::PushItemWidth(fieldWidth);
        if (ImGui::DragFloat("##PosX", &pos.x, 0.1f)) { transform->SetLocalPosition(pos); }
        ImGui::SameLine();
        if (ImGui::DragFloat("##PosY", &pos.y, 0.1f)) { transform->SetLocalPosition(pos); }
        ImGui::SameLine();
        if (ImGui::DragFloat("##PosZ", &pos.z, 0.1f)) { transform->SetLocalPosition(pos); }
        ImGui::PopItemWidth();
    }

    {   // Rotation
        glm::vec3 rotDeg = glm::degrees(transform->GetLocalEulerAngles());

        ImGui::AlignTextToFramePadding();
        ImGui::TextUnformatted("Rotation");
        ImGui::SameLine(LABEL_WIDTH);

        ImGui::PushItemWidth(fieldWidth);

        bool isChanged = false;
        if (ImGui::DragFloat("##RotX", &rotDeg.x, 0.1f)) { isChanged = true; }
        ImGui::SameLine();
        if (ImGui::DragFloat("##RotY", &rotDeg.y, 0.1f)) { isChanged = true; }
        ImGui::SameLine();
        if (ImGui::DragFloat("##RotZ", &rotDeg.z, 0.1f)) { isChanged = true; }
        if (isChanged) { transform->SetLocalEulerAngles(glm::radians(rotDeg)); }

        ImGui::PopItemWidth();
    }

    {   // Scale
        glm::vec3 scale = transform->GetLocalScale();
        ImGui::AlignTextToFramePadding();
        ImGui::TextUnformatted("Scale");
        ImGui::SameLine(LABEL_WIDTH);

        ImGui::PushItemWidth(fieldWidth);
        if (ImGui::DragFloat("##ScaleX", &scale.x, 0.1f, 0.001f, 1000.0f)) { transform->SetLocalScale(scale); }
        ImGui::SameLine();
        if (ImGui::DragFloat("##ScaleY", &scale.y, 0.1f, 0.001f, 1000.0f)) { transform->SetLocalScale(scale); }
        ImGui::SameLine();
        if (ImGui::DragFloat("##ScaleZ", &scale.z, 0.1f, 0.001f, 1000.0f)) { transform->SetLocalScale(scale); }
        ImGui::PopItemWidth();
    }

    ImGui::Unindent(16.0f);
}
