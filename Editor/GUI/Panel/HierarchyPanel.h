#pragma once

#include<Editor/GUI/Panel/Panel.h>
#include<Game/Common/FuncTable.h>
#include<Game/Scene/Scene.h>
#include<Game/Component/Camera.h>
#include<Game/Component/Metadata.h>
#include<Game/Component/Parent.h>
#include<Game/Component/MeshRenderer.h>
#include<Asset/Common/Mesh.h>

#include<functional>

namespace Pitaya::Editor
{
    class HierarchyPanel : public Panel
    {
        friend class Pitaya::Editor::GUI;

    private:
        explicit HierarchyPanel(std::string_view name = "Hierarchy")
            : Panel(name) { }
        ~HierarchyPanel() override = default;

    public:
        HierarchyPanel(const HierarchyPanel&) = delete;
        HierarchyPanel& operator=(const HierarchyPanel&) = delete;

    private:
        void OnImGuiRender() override;

    private:
        void DrawSceneTree();
        void DrawEntityNode(Pitaya::Game::Scene*, entt::entity);
        void DrawEmptyPopup();
    };
}
