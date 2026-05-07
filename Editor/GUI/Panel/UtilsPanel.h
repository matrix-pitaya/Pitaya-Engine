#pragma once

#include<Editor/GUI/Panel/Panel.h>

namespace Pitaya::Editor
{
    class UtilsPanel final : public Panel
    {
        friend class Pitaya::Editor::GUI;
    private:
        enum class SettingsTab : uint32_t
        {
            OfflinePreBaking
        };

    private:
        explicit UtilsPanel(std::string_view name = "Utils")
            : Panel(name) { }
        ~UtilsPanel() override = default;

    public:
        UtilsPanel(const UtilsPanel&) = delete;
        UtilsPanel& operator=(const UtilsPanel&) = delete;
        UtilsPanel(UtilsPanel&&) = delete;
        UtilsPanel& operator=(UtilsPanel&&) = delete;

    private:
        void OnImGuiRender() override;
        ImGuiWindowFlags GetWindowFlags() override
        {
            return ImGuiWindowFlags_NoCollapse;
        }

    private:
        void DrawOfflinePreBakingTool();

    private:
        SettingsTab selectedSettingsTab = SettingsTab::OfflinePreBaking;
    };
}
