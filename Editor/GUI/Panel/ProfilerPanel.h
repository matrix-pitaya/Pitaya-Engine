#pragma once

#include<Editor/GUI/Panel/Panel.h>


namespace Pitaya::Editor
{
    class ProfilerPanel : public Panel
    {
        friend class Pitaya::Editor::GUI;
    private:
        explicit ProfilerPanel(std::string_view name = "Profiler")
            : Panel(name) {}
        ~ProfilerPanel() override = default;

    public:
        ProfilerPanel(const ProfilerPanel&) = delete;
        ProfilerPanel& operator=(const ProfilerPanel&) = delete;
        ProfilerPanel(ProfilerPanel&&) = delete;
        ProfilerPanel& operator=(ProfilerPanel&&) = delete;

    public:
        void OnImGuiRender() override;
    };
}
