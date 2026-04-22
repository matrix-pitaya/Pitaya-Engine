#include<Editor/GUI/Panel/ProfilerPanel.h>
#include<Editor/Editor.h>

void Pitaya::Editor::ProfilerPanel::OnImGuiRender()
{
    // 帧率分析器
    if (ImGui::CollapsingHeader("CPU Time", ImGuiTreeNodeFlags_DefaultOpen))
    {
        Pitaya::Editor::TimeState state = Pitaya::Editor::Editor::Instance().GetProfiler().GetTimeState();

            float rawMsPerFrame = state.UnscaledDeltaTime * 1000.0f;
        if (rawMsPerFrame <= 0.0f) { rawMsPerFrame = 0.1f; }

        static float smoothedMs = 0.0f;
        if (smoothedMs == 0.0f) { smoothedMs = rawMsPerFrame; }
        smoothedMs = smoothedMs + (rawMsPerFrame - smoothedMs) * 0.1f;
        float displayFps = 1000.0f / smoothedMs;

        ImGui::Text("FPS: %.1f", displayFps);
        ImGui::Text("Average Frame: %.3f ms", smoothedMs);
        ImGui::Text("Current Frame: %.3f ms", rawMsPerFrame);
        ImGui::Text("TimeScale %.2f", state.TimeScale);
        ImGui::Text("Seconds %.1f", state.Seconds);

        constexpr const int frameCount = 150;
        static float values[frameCount] = {};
        static int valueOffset = 0;
        values[valueOffset] = rawMsPerFrame;
        valueOffset = (valueOffset + 1) % frameCount;

        ImVec2 graphSize(ImGui::GetContentRegionAvail().x, 80.0f);
        ImGui::InvisibleButton("##GraphRect", graphSize);
        if (ImGui::IsItemVisible())
        {
            ImDrawList* drawList = ImGui::GetWindowDrawList();
            ImVec2 p0 = ImGui::GetItemRectMin(); // 图表左上角坐标
            ImVec2 p1 = ImGui::GetItemRectMax(); // 图表右下角坐标

            // 绘制图表背景
            drawList->AddRectFilled(p0, p1, IM_COL32(40, 40, 40, 255));
            drawList->AddRect(p0, p1, IM_COL32(100, 100, 100, 255)); // 边框

            float maxMs = 33.3f; // Y轴的最高值 (图表顶部的最大毫秒数)，固定成33ms
            float barWidth = graphSize.x / frameCount; // 每根柱子的宽度
            for (int i = 0; i < frameCount; ++i)
            {
                // 注意循环缓冲区的数据读取顺序 从最老的数据开始读到最新的数据
                int index = (valueOffset + i) % frameCount;
                float v = values[index];

                float barHeight = (v / maxMs) * graphSize.y;
                if (barHeight > graphSize.y) barHeight = graphSize.y; // 截断超过顶部的部分

                // 计算具体坐标
                ImVec2 barP0 = ImVec2(p0.x + i * barWidth, p1.y - barHeight);
                ImVec2 barP1 = ImVec2(p0.x + (i + 1) * barWidth - 1.0f, p1.y); // -1.0f产生一点柱子间的缝隙

                // 根据当前这单个像素（这一帧）的耗时决定它的颜色
                // 绿色：50帧以上       黄色：30-50帧       红色：低于30帧（严重掉帧）
                ImU32 barColor = v <= 20.0f ? IM_COL32(50, 205, 50, 255) :
                    (v <= 33.4f ? IM_COL32(255, 215, 0, 255) : IM_COL32(220, 20, 60, 255));

                // 画出这个柱体（单帧线）
                if (barHeight > 1.0f) { drawList->AddRectFilled(barP0, barP1, barColor); }
            }

            // 在 20.0ms 的位置画一条辅助横线
            float line50y = p1.y - (20.0f / maxMs) * graphSize.y;
            drawList->AddLine(ImVec2(p0.x, line50y), ImVec2(p1.x, line50y), IM_COL32(255, 255, 255, 100));
        }
    }

    ImGui::Separator();

    if (ImGui::CollapsingHeader("GPU Time", ImGuiTreeNodeFlags_DefaultOpen))
    {
        //TODO 完成GPU时间间隔分析
    }

    ImGui::Separator();

    // 渲染器状态
    if (ImGui::CollapsingHeader("Renderer", ImGuiTreeNodeFlags_DefaultOpen))
    {
        //TODO 记录渲染的三角形面数等

        //TODO 移动到首选项监视面板

    }

    ImGui::Separator();

    // 系统状态
    if (ImGui::CollapsingHeader("System", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::Text("Hardware: Pitaya Engine Core");
        ImGui::Text("Pitaya Version: %d.%d.%d", PITAYA_VERSION / 100, (PITAYA_VERSION / 10) % 10, PITAYA_VERSION % 10);
        ImGui::Text("ImGui Version: %s", IMGUI_VERSION);
    }
}