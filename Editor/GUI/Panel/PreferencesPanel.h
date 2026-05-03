#pragma once

#include<Editor/GUI/Panel/Panel.h>

namespace Pitaya::Editor
{
    class PreferencesPanel final : public Panel
    {
        friend class Pitaya::Editor::GUI;
    private:
        enum class SettingsTab : uint8_t
        {
            General,
            ExternalTools,
            Theme,
            Physics
        };

    private:
        explicit PreferencesPanel(std::string_view name = "Preferences")
            : Panel(name) {}
        ~PreferencesPanel() override = default;

    public:
        PreferencesPanel(const PreferencesPanel&) = delete;
        PreferencesPanel& operator=(const PreferencesPanel&) = delete;
        PreferencesPanel(PreferencesPanel&&) = delete;
        PreferencesPanel& operator=(PreferencesPanel&&) = delete;

    private:
        void OnImGuiRender() override
        {
            ImGui::SetNextWindowSizeConstraints(ImVec2(600, 400), ImVec2(FLT_MAX, FLT_MAX));

            ImVec2 standardPadding = ImGui::GetStyle().WindowPadding;
            if (standardPadding.x == 0 && standardPadding.y == 0) { standardPadding = ImVec2(8.0f, 8.0f); }
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, standardPadding);

            // ImGuiTableFlags_ScrollY: 允许外层处理滚动（可选）
            ImGuiTableFlags tableFlags = ImGuiTableFlags_Resizable | ImGuiTableFlags_BordersInnerV;

            if (ImGui::BeginTable("PreferencesSplit", 2, tableFlags, ImGui::GetContentRegionAvail()))
            {
                // 配置第 0 列 (左侧)：使用拉伸模式，初始权重设为 0.25 (即代表默认占总宽度的 25%)
                ImGui::TableSetupColumn("LeftPane", ImGuiTableColumnFlags_WidthStretch, 0.25f);
                // 配置第 1 列 (右侧)：使用拉伸模式，初始权重设为 0.75 (默认占 75%)
                ImGui::TableSetupColumn("RightPane", ImGuiTableColumnFlags_WidthStretch, 0.75f);

                ImGui::TableNextRow();

                // 渲染左侧列
                ImGui::TableSetColumnIndex(0);
                ImGui::BeginChild("LeftChild", ImVec2(0, 0), false, 0); // 在单元格内部使用 Child 填满高度 左侧菜单项多的时候可以独立滚动
                if (ImGui::Selectable("General", selectedTab == SettingsTab::General)) { selectedTab = SettingsTab::General; }
                if (ImGui::Selectable("External Tools", selectedTab == SettingsTab::ExternalTools)) { selectedTab = SettingsTab::ExternalTools; }
                if (ImGui::Selectable("Theme & Colors", selectedTab == SettingsTab::Theme)) { selectedTab = SettingsTab::Theme; }
                if (ImGui::Selectable("Physics", selectedTab == SettingsTab::Physics)) { selectedTab = SettingsTab::Physics; }
                ImGui::EndChild();

                // 渲染右侧列
                ImGui::TableSetColumnIndex(1);
                ImGui::BeginChild("RightChild", ImVec2(0, 0), false, 0);
                ImGui::Dummy(ImVec2(0, 2.0f));  // 人为制造一点边距 让文字不要贴着表格线
                ImGui::Indent(4.0f);
                switch (selectedTab)
                {
                    case SettingsTab::General:
                        DrawGeneralSettingsTab();
                        break;

                    case SettingsTab::ExternalTools:
                        DrawExternalToolsSettingsTab();
                        break;


                    case SettingsTab::Theme:
                        DrawThemeSettingsTab();
                        break;

                    case SettingsTab::Physics:
                        DrawPhysicsSettingsTab();
                        break;
                }
                ImGui::Unindent(4.0f);
                ImGui::EndChild();

                ImGui::EndTable();
            }

            ImGui::PopStyleVar();
        }
        ImGuiWindowFlags GetWindowFlags() override
        {
            return ImGuiWindowFlags_NoCollapse;
        }

    private:
        inline void DrawGeneralSettingsTab()
        {
            ImGui::TextUnformatted("General Settings");
            ImGui::Separator();
            ImGui::Spacing();

            ImGui::Checkbox("Enable Auto Save", &m_AutoSave);

            ImGui::BeginDisabled(!m_AutoSave);
            ImGui::SliderInt("Auto Save Interval (Mins)", &m_AutoSaveInterval, 1, 60);
            ImGui::EndDisabled();

            ImGui::Spacing();
            if (ImGui::Button("Reset Defaults")) 
            {
                m_AutoSave = true;
                m_AutoSaveInterval = 5;
            }
        }
        inline void DrawExternalToolsSettingsTab()
        {
            ImGui::TextUnformatted("External Tools");
            ImGui::Separator();
            ImGui::Spacing();

            const char* editors[] = { "Visual Studio", "Visual Studio Code", "Rider" };
            ImGui::Combo("Script Editor", &m_ExternalScriptEditor, editors, IM_ARRAYSIZE(editors));

            ImGui::Spacing();
            ImGui::Button("Regenerate project files");
        }
        inline void DrawThemeSettingsTab()
        {
            ImGui::TextUnformatted("Editor Theme");
            ImGui::Separator();
            ImGui::Spacing();

            const char* themes[] = { "Dark" };
            if (ImGui::Combo("Theme", &m_CurrentTheme, themes, IM_ARRAYSIZE(themes)))
            {
                if (m_CurrentTheme == 0) { ImGui::StyleColorsDark(); }
                if (m_CurrentTheme == 1) { ImGui::StyleColorsLight(); }
            }
        }
        inline void DrawPhysicsSettingsTab()
        {
            ImGui::TextUnformatted("Physics");
            ImGui::Separator();
            ImGui::Spacing();

            ImGui::DragFloat3("Gravity", m_DefaultGravity, 0.1f);
        }

    private:
        SettingsTab selectedTab = SettingsTab::General;

        // ============ 模拟的配置数据 (实战中应从 Config 系统读取) ============
        bool  m_AutoSave = true;
        int   m_AutoSaveInterval = 5; // 分钟
        int   m_CurrentTheme = 0;     // 0: Dark, 1: Light
        int   m_ExternalScriptEditor = 0; // 0: VS, 1: VS Code, 2: Rider
        float m_DefaultGravity[3] = { 0.0f, -9.81f, 0.0f };
    };
}