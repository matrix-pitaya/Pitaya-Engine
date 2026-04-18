#pragma once

#include<Editor/GUI/Panel/ViewportPanel.h>
#include<Editor/GUI/IconFontCppHeaders/IconsFontAwesome6.h>
#include<Editor/GUI/ImGui/ImGuizmo/ImGuizmo.h>
#include<Editor/Common/TransformTool.h>
#include<entt/entt.hpp>

namespace Pitaya::Editor
{
    class SceneViewportPanel : public ViewportPanel
    {
        friend class GUI;
    private:
        explicit SceneViewportPanel(std::string_view name = "Scene")
            : ViewportPanel(name) { }
        ~SceneViewportPanel() override = default;

    public:
        SceneViewportPanel(const SceneViewportPanel&) = delete;
        SceneViewportPanel& operator=(const SceneViewportPanel&) = delete;
        SceneViewportPanel(SceneViewportPanel&&) = delete;
        SceneViewportPanel& operator=(SceneViewportPanel&&) = delete;

    public:
        bool Initialize() override
        {
            RT = Pitaya::Asset::LoadAsset<Pitaya::Asset::RenderTarget>(Pitaya::Asset::RenderTarget::Editor);
            return true;
        }

    private:
        void OnImGuiRender() override
        {
            DrawMenuBar();
            ViewportPanel::OnImGuiRender();
            DrawGizmos();
        }
        ImGuiWindowFlags GetWindowFlags() override
        {
            return ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_MenuBar;
        }

    private:
		void DrawMenuBar();
        void DrawGizmos();
    };
}
