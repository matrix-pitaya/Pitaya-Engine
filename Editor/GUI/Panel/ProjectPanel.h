#pragma once

#include<Editor/GUI/Panel/Panel.h>

namespace Pitaya::Editor
{
    class ProjectPanel : public Panel
    {
        friend class Pitaya::Editor::GUI;
    private:
        explicit ProjectPanel(std::string_view name = "Project")
            : Panel(name) {}
        ~ProjectPanel() override = default;

    public:
        ProjectPanel(const ProjectPanel&) = delete;
        ProjectPanel& operator=(const ProjectPanel&) = delete;
        ProjectPanel(ProjectPanel&&) = delete;
        ProjectPanel& operator=(ProjectPanel&&) = delete;

    public:
        void OnImGuiRender() override
        {

        }
    };
}