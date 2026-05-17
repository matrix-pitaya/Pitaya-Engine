#include<Editor/GUI/Panel/UtilsPanel.h>
#include<Editor/GUI/FileDialog/Tinyfiledialogs/tinyfiledialogs.h>
#include<Editor/Editor.h>
#include<Log/Common/FuncTable.h>
#include<Import/Import.h>
#include<Core/Utils/String.h>

#include<string>
#include<filesystem>
#include<fstream>

namespace
{
	//离线预烘培用于将复杂模型的.obj格式 转化为二进制.war进行存储
	bool OfflinePreBaking_StaticMesh(const Pitaya::Import::StaticMeshImportResult& result, const std::filesystem::path& warPath)
	{
		std::ofstream out(warPath, std::ios::binary);
		if (!out.is_open()) { return false; }

		// 写基础信息
		out.write(reinterpret_cast<const char*>(&result.GUID), sizeof(Pitaya::Core::GUID));
		out.write(reinterpret_cast<const char*>(&result.IsValid), sizeof(bool));
		out.write(reinterpret_cast<const char*>(&result.BoundingBox), sizeof(Pitaya::Core::AABB));

		// 写入浮点顶点数组
		uint32_t vertexCount = static_cast<uint32_t>(result.Vertices.size());
		out.write(reinterpret_cast<const char*>(&vertexCount), sizeof(uint32_t));
		if (vertexCount > 0) { out.write(reinterpret_cast<const char*>(result.Vertices.data()), vertexCount * sizeof(float)); }

		// 写入索引数组
		uint32_t indexCount = static_cast<uint32_t>(result.Indices.size());
		out.write(reinterpret_cast<const char*>(&indexCount), sizeof(uint32_t));
		if (indexCount > 0) { out.write(reinterpret_cast<const char*>(result.Indices.data()), indexCount * sizeof(uint32_t)); }

		// 写入 SubMesh 数组 因为其内部只有基础数据所以直接写整块内存
		uint32_t subMeshCount = static_cast<uint32_t>(result.SubMeshs.size());
		out.write(reinterpret_cast<const char*>(&subMeshCount), sizeof(uint32_t));
		if (subMeshCount > 0) { out.write(reinterpret_cast<const char*>(result.SubMeshs.data()), subMeshCount * sizeof(Pitaya::Asset::Mesh::SubMesh)); }

		// 写入材质 GUID 数组
		uint32_t matCount = static_cast<uint32_t>(result.MaterialGUIDs.size());
		out.write(reinterpret_cast<const char*>(&matCount), sizeof(uint32_t));
		if (matCount > 0) { out.write(reinterpret_cast<const char*>(result.MaterialGUIDs.data()), matCount * sizeof(Pitaya::Core::GUID)); }

		out.close();
		return true;
	}
    bool OfflinePreBaking_HDR(const std::filesystem::path& hdrPath, const std::filesystem::path& warPath)
    {
        Pitaya::Import::Texture2DImportResult result;
        if (!Pitaya::Import::Import(Pitaya::Core::GUID(), hdrPath, true, false, result))
        {
            Pitaya::Log::Error("offline prebaking HDR fail, path:" + hdrPath.string());
            return false; 
        }

        std::ofstream out(warPath, std::ios::binary);
        if (!out.is_open())
        {
            return false;
        }

        out.write(reinterpret_cast<const char*>(&result.GUID), sizeof(Pitaya::Core::GUID));
        out.write(reinterpret_cast<const char*>(&result.Width), sizeof(int));
        out.write(reinterpret_cast<const char*>(&result.Height), sizeof(int));
        out.write(reinterpret_cast<const char*>(&result.Channels), sizeof(int));

        out.write(reinterpret_cast<const char*>(&result.IsGenerateMipmap), sizeof(bool));
        out.write(reinterpret_cast<const char*>(&result.Usage), sizeof(Pitaya::Asset::TextureUsage));
        out.write(reinterpret_cast<const char*>(&result.isNearest), sizeof(bool));

        uint32_t dataByteSize = static_cast<uint32_t>(result.Data.size());
        out.write(reinterpret_cast<const char*>(&dataByteSize), sizeof(uint32_t)); 
        if (dataByteSize > 0) { out.write(reinterpret_cast<const char*>(result.Data.data()), dataByteSize * sizeof(uint8_t)); }

        out.close();
        return true;
    }
}

void Pitaya::Editor::UtilsPanel::OnImGuiRender()
{
    ImGui::SetNextWindowSizeConstraints(ImVec2(600, 400), ImVec2(FLT_MAX, FLT_MAX));
    ImVec2 standardPadding = ImGui::GetStyle().WindowPadding;
    if (standardPadding.x == 0 && standardPadding.y == 0) { standardPadding = ImVec2(8.0f, 8.0f); }
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, standardPadding);
    ImGuiTableFlags tableFlags = ImGuiTableFlags_Resizable | ImGuiTableFlags_BordersInnerV;

    // 创建充满可用区域的二列表格 
    if (ImGui::BeginTable("UtilsSplit", 2, tableFlags, ImGui::GetContentRegionAvail()))
    {
        ImGui::TableSetupColumn("LeftPane", ImGuiTableColumnFlags_WidthStretch, 0.25f);   // 配置第 0 列 (左侧)，比例约占 25% (0.25f)
        ImGui::TableSetupColumn("RightPane", ImGuiTableColumnFlags_WidthStretch, 0.75f);   // 配置第 1 列 (右侧)，比例约占 75% (0.75f)

        ImGui::TableNextRow();

        // 左侧列：工具列表
        ImGui::TableSetColumnIndex(0);
        ImGui::BeginChild("LeftChild", ImVec2(0, 0), false, 0); // 在单元格内开启子窗口以支持独立滚动
        if (ImGui::Selectable("Offline PreBaking Tool", selectedSettingsTab == SettingsTab::OfflinePreBaking)) { selectedSettingsTab = SettingsTab::OfflinePreBaking; }
        ImGui::EndChild();

        // 右侧列：具体面板 
        ImGui::TableSetColumnIndex(1);
        ImGui::BeginChild("RightChild", ImVec2(0, 0), false, 0);
        ImGui::Dummy(ImVec2(0, 2.0f));  // 制造一点垂直上边距，不让文字死死贴在一起
        ImGui::Indent(4.0f);            // 往右缩进，提供留白空间

        switch (selectedSettingsTab)
        {
            case SettingsTab::OfflinePreBaking:
                DrawOfflinePreBakingTool();
                break;

            default:
                ImGui::Text("Error tab, there is nothing here yet.");
                break;
        }

        ImGui::Unindent(4.0f);
        ImGui::EndChild();

        ImGui::EndTable();
    }

    ImGui::PopStyleVar();
}
void Pitaya::Editor::UtilsPanel::DrawOfflinePreBakingTool()
{
    static char inputObjPath[512] = { };
    static char outputPath[512] = { };

    ImGui::TextUnformatted("Hdr to War Tool");
    ImGui::Separator();
    ImGui::Spacing();

    ImGui::AlignTextToFramePadding();
    ImGui::Text("Input Hdr File:");
    ImGui::SameLine();
    ImGui::InputText("##Input", inputObjPath, sizeof(inputObjPath));
    ImGui::SameLine();
    if (ImGui::Button("Browse...##InputBtn"))
    {
        const char* filterPatterns[1] = { "*.hdr" };
        const char* selectedPath = tinyfd_openFileDialog(
            "请选择要转换的 .hdr 文件", "", 1, filterPatterns, "OBJ Files (*.hdr)", 0);
        if (selectedPath) { Pitaya::Core::CopyStringToCharArray(selectedPath, inputObjPath); }
    }

    ImGui::Spacing();

    ImGui::AlignTextToFramePadding();
    ImGui::Text("Output War Dir:");
    ImGui::SameLine();
    ImGui::InputText("##Output", outputPath, sizeof(outputPath));
    ImGui::SameLine();
    if (ImGui::Button("Browse...##OutputBtn"))
    {
        const char* filterPatterns[1] = { "*.war" };
        const char* savedPath = tinyfd_saveFileDialog(
            "请选择输出位置", "output.war", 1, filterPatterns, "WAR Files (*.war)");
        if (savedPath) { Pitaya::Core::CopyStringToCharArray(savedPath, outputPath); }
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    if (ImGui::Button("Offline PreBaking", ImVec2(240, 30)))
    {
        std::string_view inPath(inputObjPath);
        std::string_view outPath(outputPath);
        if (!inPath.empty() && !outPath.empty())
        {
            OfflinePreBaking_HDR(inPath, outPath);
        }
    }
}