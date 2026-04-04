#pragma once

#include<Editor/GUI/Panel/Panel.h>
#include<Core/Asset/Asset.h>
#include<Asset/Common/RenderTarget.h>
#include<Asset/Common/FuncTable.h>

namespace Pitaya::Editor
{
    class ViewportPanel : public Panel
    {
        friend class Pitaya::Editor::GUI;
    private:
        explicit ViewportPanel(std::string_view name = "Viewport")
            : Panel(name) { }
        ~ViewportPanel() override = default;

    public:
        ViewportPanel(const ViewportPanel&) = delete;
        ViewportPanel& operator=(const ViewportPanel&) = delete;
        ViewportPanel(ViewportPanel&&) = delete;
        ViewportPanel& operator=(ViewportPanel&&) = delete;

    private:
        inline ImVec2 GetSize() const 
        { 
            return viewportSize;
        }

    private:
        void OnImGuiRender() override
        {
            if (ImGui::BeginMenuBar())
            {
                if (ImGui::BeginMenu("AspectRatio"))
                {

                    ImGui::EndMenu();
                }

                ImGui::EndMenuBar();
            }

            ImVec2 availSize = ImGui::GetContentRegionAvail();
            if (availSize.x <= 0 || availSize.y <= 0) { return; }

            ImVec2 renderSize;
            float windowAspect = availSize.x / availSize.y;
            if (windowAspect > aspectRatio)
            {
                renderSize.y = availSize.y;
                renderSize.x = renderSize.y * aspectRatio;
            }
            else
            {
                renderSize.x = availSize.x;
                renderSize.y = renderSize.x / aspectRatio;
            }

            viewportSize = renderSize;
            float offsetX = (availSize.x - renderSize.x) * 0.5f;
            float offsetY = (availSize.y - renderSize.y) * 0.5f;

            if (!textureId && RT.IsReady()) 
            { 
                textureId = (void*)(ImTextureID)(intptr_t)RT->FinalColorAttachment; 
            }

            ImVec2 cursorPos = ImGui::GetCursorPos();
            if (textureId)
            {
                ImGui::SetCursorPos(ImVec2(cursorPos.x + offsetX, cursorPos.y + offsetY));
                ImGui::Image(textureId, renderSize, ImVec2(0, 1), ImVec2(1, 0));
            }
            else
            {
                ImGui::SetCursorPos(ImVec2(cursorPos.x + availSize.x * 0.5f - 80.0f, cursorPos.y + availSize.y * 0.5f));
                ImGui::Text("No Framebuffer Texture");
            }

            // TODO 这里还可以处理 Gizmos (坐标轴) 的绘制逻辑
            // 注意：画 Gizmo 时，位置和大小应使用上面的 renderSize 和偏移后的 Cursor位置*
        }
        ImGuiWindowFlags GetWindowFlags() override
        {
            return ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_MenuBar;
        }
        bool Initialize() override
        {
            RT = Pitaya::Asset::LoadAsset<Pitaya::Asset::RenderTarget>(name == "Scene" ? Pitaya::Asset::RenderTarget::Editor : Pitaya::Asset::RenderTarget::Game);
            return true;
        }
        void Release() override
        {
            RT = nullptr;
        }

    private:
        void* textureId = nullptr;
        float aspectRatio = 16.0f / 9.0f;
        ImVec2 viewportSize = ImVec2(0, 0);
        Pitaya::Core::Asset<Pitaya::Asset::RenderTarget> RT = nullptr;
    };
}
