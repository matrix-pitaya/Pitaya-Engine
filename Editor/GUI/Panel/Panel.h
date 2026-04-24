#pragma once

#include<Editor/GUI/ImGui/imgui.h>
#include<Editor/GUI/ImGui/imgui_internal.h>

#include<string>

namespace Pitaya::Editor
{
    class GUI;
	class Panel
	{
        friend class Pitaya::Editor::GUI;
    protected:
        explicit Panel(std::string_view name)
            : name(name) {}
        virtual ~Panel() = default;

    public:
        Panel(const Panel&) = delete;
        Panel& operator=(const Panel&) = delete;
        Panel(Panel&&) = delete;
        Panel& operator=(Panel&&) = delete;

    private:
        void Draw()
        {
            if (!isOpen) 
            { 
                isVisable = false;
                return;
            }

            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
            isVisable = ImGui::Begin(name.c_str(), &isOpen, GetWindowFlags());
            if (isVisable)
            {
                isVisable = !ImGui::GetCurrentWindow()->SkipItems;
                isFocused = ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows);
                isHovered = ImGui::IsWindowHovered(ImGuiHoveredFlags_RootAndChildWindows);
                if (isVisable) { OnImGuiRender(); }
            }
            ImGui::End();
            ImGui::PopStyleVar();
        }

    private:
        inline void Open() noexcept
        {
            isOpen = true; 
        }
        inline void Close() noexcept
        { 
            isOpen = false;
        }
        inline void Toggle() noexcept
        { 
            isOpen = !isOpen;
        }

    public:
        inline bool GetOpenState() const noexcept
        {
            return isOpen; 
        }
        inline bool GetIsFocused() const noexcept
        {
            return isFocused;
        }
        inline bool GetIsHovered() const noexcept
        {
            return isHovered;
        }
        inline bool GetIsVisable() const noexcept
        {
            return isVisable;
        }
        inline std::string_view GetName() const noexcept
        {
            return name;
        }

    protected:
        //子类必须实现实际绘画逻辑 画具体的按钮、文本
        virtual void OnImGuiRender() = 0;           

        //子类可以覆盖这个来改变窗口属性
        virtual ImGuiWindowFlags GetWindowFlags()   
        {
            return ImGuiWindowFlags_None;
        }

    protected:
        std::string name;
        bool isOpen = true;
        bool isFocused = false;
        bool isHovered = false;
        bool isVisable = false;
	};
}
