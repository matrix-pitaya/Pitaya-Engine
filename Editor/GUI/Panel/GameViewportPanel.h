#pragma once

#include<Editor/GUI/Panel/ViewportPanel.h>

namespace Pitaya::Editor
{
    class GameViewportPanel final : public ViewportPanel
    {
        friend class GUI;
    private:
        explicit GameViewportPanel(std::string_view name = "Game")
            : ViewportPanel(name) {}
        ~GameViewportPanel() override = default;

    public:
        GameViewportPanel(const GameViewportPanel&) = delete;
        GameViewportPanel& operator=(const GameViewportPanel&) = delete;
        GameViewportPanel(GameViewportPanel&&) = delete;
        GameViewportPanel& operator=(GameViewportPanel&&) = delete;
    };
}