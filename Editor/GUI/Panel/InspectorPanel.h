#pragma once

#include<Editor/GUI/Panel/Panel.h>
#include<Editor/GUI/IconFontCppHeaders/IconsFontAwesome6.h>
#include<Game/Component/Tag.h>
#include<Game/Component/Transform.h>

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
        void OnImGuiRender() override;

    private:
        void DrawEntity();
        void DrawFile();

    private:
        void DrawTag(Pitaya::Game::Tag* tag);
        void DrawTransform(Pitaya::Game::Transform* transform);
    };
}
