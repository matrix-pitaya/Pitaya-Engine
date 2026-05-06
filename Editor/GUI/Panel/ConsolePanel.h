#pragma once

#include<Core/Color/Color.h>
#include<Core/Thread/Thread.h>
#include<Core/Container/ThreadSafe/ThreadSafeQueue.h>
#include<Core/Utils/Time.h>
#include<Editor/GUI/Panel/Panel.h>
#include<Log/Common/LogLevel.h>
#include<Thread/Common/FuncTable.h>

namespace Pitaya::Editor
{
	class ConsolePanel final : public Panel
	{
		friend class  Pitaya::Editor::GUI;
    private:
        struct ConsoleMessage
        {
            ImVec4 color;
            std::string time;
            std::string thread;
            std::string message;
            std::string_view level;
        };

    private:
        explicit ConsolePanel(std::string_view name = "Console")
            : Panel(name) { }
        ~ConsolePanel() override = default;

    public:
        ConsolePanel(const ConsolePanel&) = delete;
        ConsolePanel& operator=(const ConsolePanel&) = delete;
        ConsolePanel(ConsolePanel&&) = delete;
        ConsolePanel& operator=(ConsolePanel&&) = delete;

    public:
        void Console(Pitaya::Log::LogLevel level, std::string_view message)
        {
            ConsoleMessage msg;
            msg.message = message;
            msg.thread = "[" + Pitaya::Thread::GetThreadName(Pitaya::Core::Thread::GetCurrentThreadId()) + "]";
            msg.time = "[" + Pitaya::Core::Date() + "]";
            switch (level)
            {
                case Pitaya::Log::LogLevel::Info:   
                    msg.color = ImVec4(Pitaya::Core::Color::White.r, Pitaya::Core::Color::White.g, Pitaya::Core::Color::White.b, Pitaya::Core::Color::White.a);
                    msg.level = "[Info]";
                    break;
                
                case Pitaya::Log::LogLevel::Debug:   
                    msg.color = ImVec4(Pitaya::Core::Color::Green.r, Pitaya::Core::Color::Green.g, Pitaya::Core::Color::Green.b, Pitaya::Core::Color::Green.a);
                    msg.level = "[Debug]";
                    break;
                
                case Pitaya::Log::LogLevel::Warning: 
                    msg.color = ImVec4(Pitaya::Core::Color::Yellow.r, Pitaya::Core::Color::Yellow.g, Pitaya::Core::Color::Yellow.b, Pitaya::Core::Color::Yellow.a);    
                    msg.level = "[Warning]";
                    break;
                
                case Pitaya::Log::LogLevel::Error:   
                    msg.color = ImVec4(Pitaya::Core::Color::Red.r, Pitaya::Core::Color::Red.g, Pitaya::Core::Color::Red.b, Pitaya::Core::Color::Red.a);                
                    msg.level = "[Error]";
                    break;
            }
            queue.Push(std::move(msg));
        }

    private:
        void OnImGuiRender() override
        {
            std::queue<ConsoleMessage> messages = queue.PopAll();
            while (!messages.empty())
            {
                history.push_back(std::move(messages.front()));
                messages.pop();
            }

            while (history.size() > MaxConsoleMessageCount)
            {
                history.pop_front();
            }

            float windowWidth = ImGui::GetWindowWidth();
            ImGuiStyle& style = ImGui::GetStyle();
            ImGui::SetCursorPosX(style.WindowPadding.x + 8.0f);
            if (ImGui::Button("Clear")) { history.clear(); }

            ImGui::SameLine();
            ImGui::Checkbox("Auto-Scroll", &autoScroll);

            float filterInputWidth = 350.0f;
            float filterTotalWidth = filterInputWidth + style.ItemInnerSpacing.x + ImGui::CalcTextSize("Filter").x;
            float rightX = windowWidth - filterTotalWidth - style.WindowPadding.x;
            float afterCheckboxX = ImGui::GetCursorPosX() + style.ItemSpacing.x;
            if (rightX < afterCheckboxX) { rightX = afterCheckboxX; }
            ImGui::SameLine();
            ImGui::SetCursorPosX(rightX);
            textFilter.Draw("Filter", filterInputWidth);

            ImGui::Separator();
            const float footerHeightToReserve = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();

            ImGuiWindowFlags childFlags = ImGuiWindowFlags_HorizontalScrollbar;
            if (!isFocused) { childFlags |= ImGuiWindowFlags_NoScrollWithMouse; }

            ImGui::PushStyleColor(ImGuiCol_ChildBg, ImGui::GetStyle().Colors[ImGuiCol_FrameBg]);
            if (ImGui::BeginChild("ScrollingRegion", ImVec2(0, -footerHeightToReserve), false, childFlags))
            {
                for (const auto& msg : history)
                {
                    if (!textFilter.PassFilter(msg.message.c_str())) { continue; }
                    ImGui::PushStyleColor(ImGuiCol_Text, msg.color);
                    ImGui::TextUnformatted(msg.level.data());
                    ImGui::SameLine();
                    ImGui::TextUnformatted(msg.time.c_str());
                    ImGui::SameLine();
                    ImGui::TextUnformatted(msg.thread.c_str());
                    ImGui::SameLine();
                    ImGui::TextUnformatted(msg.message.c_str());
                    ImGui::PopStyleColor();
                }

                if (autoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
                {
                    ImGui::SetScrollHereY(1.0f);
                }
            }
            ImGui::EndChild();
            ImGui::PopStyleColor();
        }

    private:
        ImGuiTextFilter textFilter;

        Pitaya::Core::ThreadSafeQueue<ConsoleMessage> queue;
        std::deque<ConsoleMessage> history;
        bool autoScroll = true;

        inline static constexpr const uint32_t MaxConsoleMessageCount = 300u;
	};
}
