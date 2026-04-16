#pragma once

#include<Editor/GUI/Panel/Panel.h>
#include<Editor/GUI/IconFontCppHeaders/IconsFontAwesome6.h>
#include<Core/Utils/File.h>

#include<unordered_map>

namespace Pitaya::Editor
{
    class ProjectPanel : public Panel
    {
        friend class Pitaya::Editor::GUI;
    private:
        struct FileNode 
        {
            std::filesystem::path path;
            std::string name;
            bool isDirectory;
            bool isEmptyDir;
        };
        struct DirectoryCache
        {
            std::vector<FileNode> nodes;
            std::filesystem::file_time_type lastWriteTime;
            double nextCheckTime = 0.0f;
        };

    private:
        explicit ProjectPanel(std::string_view name = "Project")
            : Panel(name) { }
        ~ProjectPanel() override = default;

    public:
        ProjectPanel(const ProjectPanel&) = delete;
        ProjectPanel& operator=(const ProjectPanel&) = delete;
        ProjectPanel(ProjectPanel&&) = delete;
        ProjectPanel& operator=(ProjectPanel&&) = delete;

    public:
        void OnImGuiRender() override
        {
            const std::filesystem::path workspacePath = Pitaya::Core::GetWorkspace();
            if (baseDirectory != workspacePath)
            {
                baseDirectory = workspacePath;
                currentDirectory = baseDirectory;
                selectedPath = baseDirectory;
                directoryCache.clear(); // 工作区变动时一次性清空缓存
            }

            if (baseDirectory.empty())
            {
                ImGui::Text("No workspace loaded...");
                return;
            }

            // 视口宽度检测
            float panelWidth = ImGui::GetContentRegionAvail().x;
            float switchThreshold = 350.0f; // 宽度阈值

            if (panelWidth < switchThreshold)
            {
                // 窄模式 只显示树状列表
                ImGui::BeginChild("FolderTreeMode", ImVec2(0, 0), false);
                DrawDirectoryTree(baseDirectory);
                ImGui::EndChild();
            }
            else
            {
                // 宽模式 只显示大图标网格
                ImGui::BeginChild("GridContentMode", ImVec2(0, 0), false);
                DrawDirectoryContents();
                ImGui::EndChild();
            }
        }

    private:
        void DrawDirectoryTree(const std::filesystem::path& currentParent)
        {
            const auto& nodes = GetCachedDirectoryNodes(currentParent);
            for (const auto& node : nodes)
            {
                if (node.isDirectory)
                {
                    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
                    if (selectedPath == node.path) { flags |= ImGuiTreeNodeFlags_Selected; }
                    if (node.isEmptyDir) { flags |= ImGuiTreeNodeFlags_Leaf; }

                    ImGuiID nodeID = ImGui::GetID(node.path.string().c_str());
                    bool isNodeOpen = ImGui::GetStateStorage()->GetInt(nodeID, 0) != 0;
                    const char* icon = isNodeOpen ? ICON_FA_FOLDER_OPEN : ICON_FA_FOLDER;

                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.9f, 0.7f, 0.3f, 1.0f));
                    bool opened = ImGui::TreeNodeEx(node.path.string().c_str(), flags, "%s", icon);
                    ImGui::PopStyleColor();

                    ImGui::SameLine();
                    ImGui::Text("%s", node.name.c_str());

                    if (ImGui::IsItemClicked())
                    {
                        currentDirectory = node.path;
                        selectedPath = node.path;
                    }

                    if (opened)
                    {
                        DrawDirectoryTree(node.path);
                        ImGui::TreePop();
                    }
                }
                else
                {
                    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_SpanAvailWidth;
                    if (selectedPath == node.path) { flags |= ImGuiTreeNodeFlags_Selected; }

                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.7f, 0.7f, 0.7f, 1.0f));
                    ImGui::TreeNodeEx(node.path.string().c_str(), flags, "%s", ICON_FA_FILE);
                    ImGui::PopStyleColor();

                    ImGui::SameLine();
                    ImGui::Text("%s", node.name.c_str());

                    if (ImGui::IsItemClicked())
                    {
                        selectedPath = node.path;
                    }
                }
            }
        }
        void DrawDirectoryContents()
        {
            ImGui::AlignTextToFramePadding();
            bool isAtRoot = (currentDirectory == baseDirectory);
            if (isAtRoot) { ImGui::BeginDisabled(); }
            if (ImGui::Button(ICON_FA_ARROW_LEFT " Back"))
            {
                currentDirectory = currentDirectory.parent_path();
                selectedPath = currentDirectory;
            }
            if (isAtRoot) { ImGui::EndDisabled(); }

            ImGui::SameLine();

            std::filesystem::path relativePath = std::filesystem::relative(currentDirectory, baseDirectory);
            std::string pathText = (relativePath == ".") ? "Workspace" : relativePath.string();
            ImGui::Text("Path: %s", pathText.c_str());

            ImGui::Separator();

            float padding = 16.0f;
            float thumbnailSize = 64.0f;
            float cellSize = thumbnailSize + padding;
            float panelWidth = ImGui::GetContentRegionAvail().x;
            int columnCount = std::max(1, static_cast<int>(panelWidth / cellSize));

            if (ImGui::BeginTable("ContentGrid", columnCount))
            {
                const auto& nodes = GetCachedDirectoryNodes(currentDirectory);

                for (const auto& node : nodes)
                {
                    ImGui::TableNextColumn();
                    ImGui::PushID(node.path.string().c_str());

                    ImVec4 iconColor = node.isDirectory ? ImVec4(0.9f, 0.7f, 0.3f, 1.0f) : ImVec4(0.7f, 0.7f, 0.7f, 1.0f);

                    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
                    ImGui::PushStyleColor(ImGuiCol_Text, iconColor);
                    ImGui::SetWindowFontScale(3.0f);

                    const char* iconStr = node.isDirectory ? ICON_FA_FOLDER : ICON_FA_FILE;
                    if (ImGui::Button(iconStr, ImVec2(thumbnailSize, thumbnailSize)))
                    {
                        if (node.isDirectory)
                        {
                            currentDirectory = node.path;
                            selectedPath = currentDirectory;
                        }
                        else
                        {
                            selectedPath = node.path;
                        }
                    }

                    ImGui::SetWindowFontScale(1.0f);
                    ImGui::PopStyleColor(2);

                    ImGui::PushTextWrapPos(ImGui::GetCursorPosX() + thumbnailSize);

                    if (selectedPath == node.path)
                    {
                        ImGui::TextColored(ImVec4(0.2f, 0.6f, 1.0f, 1.0f), "%s", node.name.c_str());
                    }
                    else
                    {
                        ImGui::Text("%s", node.name.c_str());
                    }

                    ImGui::PopTextWrapPos();
                    ImGui::PopID();
                }
                ImGui::EndTable();
            }
        }

    private:
        const std::vector<FileNode>& GetCachedDirectoryNodes(const std::filesystem::path& dirPath)
        {
            std::string key = dirPath.string();
            auto& cache = directoryCache[key]; // 获取或自动创建缓存结构

            double currentTime = ImGui::GetTime();
            bool needsRebuild = false;

            // 如果该目录第一次加载，或者轮询时间(0.5秒)已到，则检查操作系统文件时间戳
            if (cache.nodes.empty() && cache.nextCheckTime == 0.0f)
            {
                needsRebuild = true;
            }
            else if (currentTime > cache.nextCheckTime)
            {
                cache.nextCheckTime = currentTime + 0.5f; // 限制至少0.5秒才执行一次系统调用
                try
                {
                    if (std::filesystem::exists(dirPath))
                    {
                        auto currentWriteTime = std::filesystem::last_write_time(dirPath);
                        if (currentWriteTime != cache.lastWriteTime)
                        {
                            needsRebuild = true; // 只有发生变动才触发重建！
                        }
                    }
                    else
                    {
                        cache.nodes.clear();
                    }
                }
                catch (...) {}
            }

            // 触发重建
            if (needsRebuild)
            {
                cache.nodes.clear();
                try
                {
                    for (const auto& entry : std::filesystem::directory_iterator(dirPath))
                    {
                        FileNode node;
                        node.path = entry.path();
                        node.name = node.path.filename().string();
                        node.isDirectory = entry.is_directory();
                        node.isEmptyDir = true;

                        if (node.isDirectory)
                        {
                            try
                            {
                                for (const auto& _ : std::filesystem::directory_iterator(node.path))
                                {
                                    node.isEmptyDir = false;
                                    break;
                                }
                            }
                            catch (...) {}
                        }
                        cache.nodes.push_back(node);
                    }
                }
                catch (...) {}

                std::sort(cache.nodes.begin(), cache.nodes.end(), 
                    [](const FileNode& a, const FileNode& b)
                    {
                        return (a.isDirectory != b.isDirectory) ? (a.isDirectory > b.isDirectory) : (a.name < b.name);
                    });

                try
                {
                    cache.lastWriteTime = std::filesystem::last_write_time(dirPath);
                }
                catch (...) {}

                cache.nextCheckTime = currentTime + 0.5f;
            }

            return cache.nodes;
        }

    private:
        std::filesystem::path baseDirectory;        // 工作区绝对根路径
        std::filesystem::path currentDirectory;     // 当前右侧/下方网格正在浏览的文件夹
        std::filesystem::path selectedPath;         // 当前树状图或被选中的确切目标 (文件或文件夹) 

        std::unordered_map<std::string, DirectoryCache> directoryCache; // 高速缓存池
    };
}
