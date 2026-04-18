#include<Editor/GUI/GUI.h>
#include<Editor/Common/EngineState.h>
#include<Editor/GUI/ImGui/imgui.h>
#include<Editor/GUI/ImGui/Backends/imgui_impl_glfw.h>
#include<Editor/GUI/ImGui/Backends/imgui_impl_opengl3.h>
#include<Editor/GUI/ImGui/imgui_internal.h>
#include<Editor/GUI/IconFontCppHeaders/IconsFontAwesome6.h>
#include<Log/Common/FuncTable.h>
#include<Core/Color/Color.h>
#include<Core/Utils/Memory.h>
#include<Core/Utils/File.h>
#include<Hook/def.h>

#include<GL/glew.h>
#include<GLFW/glfw3.h>

#include<mimalloc.h>
#include<exception>
#include<stdexcept>
#include<thread>
#include<chrono> 
#include<iostream>
#include<cstdio>
#include<filesystem>

#define NOMINMAX
#include<windows.h>

#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#endif

namespace
{
	FILE* ConsoleOutStream = nullptr;
	FILE* ConsoleInStream = nullptr;
	FILE* ConsoleErrStream = nullptr;
	bool IsConsoleAllocated = false;

	bool OpenConsole()
	{
		//如果之前已经创建过控制台 则不再重新创建 直接显示
		if (IsConsoleAllocated)
		{
			HWND consoleWindow = GetConsoleWindow();
			if (consoleWindow != nullptr)
			{
				ShowWindow(consoleWindow, SW_SHOW); //仅仅是显示窗口
			}
			return true;
		}

		//分配一个控制台窗口 整个引擎生命周期只做一次
		AllocConsole();
		IsConsoleAllocated = true;

		//重定向 C 语言流
		freopen_s(&ConsoleOutStream, "CONOUT$", "w", stdout);
		freopen_s(&ConsoleErrStream, "CONOUT$", "w", stderr);
		freopen_s(&ConsoleInStream, "CONIN$", "r", stdin);

		//重置 C++ 流状态
		std::cout.clear();
		std::clog.clear();
		std::cerr.clear();
		std::cin.clear();

		//设置 UTF-8 编码，防止中文乱码
		SetConsoleOutputCP(CP_UTF8);

		//开启控制台的 ANSI 颜色支持
		HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
		if (hOut != INVALID_HANDLE_VALUE)
		{
			DWORD dwMode = 0;
			if (GetConsoleMode(hOut, &dwMode))
			{
				//给当前控制台模式加上虚拟终端解析能力
				dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
				SetConsoleMode(hOut, dwMode);
			}
		}

		//禁用控制台的快速编辑模式 防止鼠标点击导致引擎卡死
		HANDLE hIn = GetStdHandle(STD_INPUT_HANDLE);
		if (hIn != INVALID_HANDLE_VALUE)
		{
			DWORD dwInMode = 0;
			if (GetConsoleMode(hIn, &dwInMode))
			{
				//去除快速编辑模式，必须配合 ENABLE_EXTENDED_FLAGS 使用
				dwInMode &= ~ENABLE_QUICK_EDIT_MODE;
				dwInMode |= ENABLE_EXTENDED_FLAGS;
				SetConsoleMode(hIn, dwInMode);
			}
		}

		//禁用右上角X按钮 设置标题
		HWND consoleWindow = GetConsoleWindow();
		if (consoleWindow != nullptr)
		{
			HMENU hMenu = GetSystemMenu(consoleWindow, FALSE);
			if (hMenu != nullptr)
			{
				DeleteMenu(hMenu, SC_CLOSE, MF_BYCOMMAND);
			}
			SetConsoleTitleA("Pitaya Engine Console");
		}
		return true;
	}
	bool CloseConsole()
	{
		HWND consoleWindow = GetConsoleWindow();
		if (consoleWindow == nullptr) { return true; }

		// 安全隐藏窗口，绝不释放资源，防止引擎后台 cout 时崩溃
		ShowWindow(consoleWindow, SW_HIDE);
		return true;
	}

	//生成文件夹的 CheckList.req 文件 记录文件夹内的所有文件路径（相对于该文件夹的路径） 供引擎加载时读取
	void GenerateCheckList(const std::filesystem::path& folder)
	{
		if (!std::filesystem::exists(folder) || !std::filesystem::is_directory(folder)) { return; }

		const std::filesystem::path checklistfile = folder / "CheckList.req";
		std::ofstream outFile(checklistfile, std::ios::out | std::ios::trunc);
		if (!outFile.is_open()) { return; }
		
		//递归遍历
		for (const auto& entry : std::filesystem::recursive_directory_iterator(folder))
		{
			//排除 CheckList.req 自身以及文件夹条目，只记录文件
			if (std::filesystem::is_regular_file(entry.status())) 
			{
				if (entry.path() == checklistfile) { continue; }

				//获取相对于 RootPath 的路径
				std::filesystem::path relPath = std::filesystem::relative(entry.path(), folder);

				//使用 generic_string 以确保路径分隔符在 Windows 下为 '/'
				outFile << relPath.generic_string() << "\n";
			}
		}

		outFile.close();
	}
}

bool Pitaya::Editor::GUI::Initialize_Main(void* nativeWindow)
{
	IMGUI_CHECKVERSION();
	auto* context = ImGui::CreateContext();
	SetStyle();
	ImGui::SetCurrentContext(context);
	ImGui_ImplGlfw_InitForOpenGL(static_cast<GLFWwindow*>(nativeWindow), true);
	imGuiContext.store(context, std::memory_order_release);
	for (auto panel : panels.Each()) { panel->Initialize(); }
	while (!isRenderReady.load(std::memory_order_acquire)){ std::this_thread::yield(); }
	return drawer.Initialize(nativeWindow);
}
bool Pitaya::Editor::GUI::Initialize_Render()
{
	ImGuiContext* context = nullptr;
	while (!(context = imGuiContext.load(std::memory_order_acquire))) { std::this_thread::yield(); }
	ImGui::SetCurrentContext(context);
	ImGui_ImplOpenGL3_Init("#version 130");
	ImGui_ImplOpenGL3_CreateDeviceObjects();
	isRenderReady.store(true, std::memory_order_release);
	return true;
}
void Pitaya::Editor::GUI::Release_Main()
{
	drawer.Release();
	for (auto panel : panels.Each()) { panel->Release(); }
	const auto start = std::chrono::steady_clock::now();
	const auto wait = std::chrono::milliseconds(2000);
	while (isRenderReady.load(std::memory_order_acquire))
	{
		auto elapsed = std::chrono::steady_clock::now() - start;
		if (elapsed > wait)
		{
			Pitaya::Log::Error("GUI Shutdown Timed Out! Force clearing...");
			break;
		}

		std::this_thread::yield();
	}
	ImGuiContext* context = imGuiContext.load(std::memory_order_acquire);
	if (context)
	{
		ImGui::SetCurrentContext(context);
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext(context);
		imGuiContext.store(nullptr, std::memory_order_release);
	}
}
void Pitaya::Editor::GUI::Release_Render()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui::SetCurrentContext(nullptr);
	isRenderReady.store(false, std::memory_order_release);
}
void Pitaya::Editor::GUI::SetStyle()
{
	static std::string imguiInPath = (Pitaya::Core::GetWorkspace() / "imgui.ini").string();
	ImGuiIO& io = ImGui::GetIO();
	io.IniFilename = imguiInPath.c_str();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	// io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;	//TODO 涉及到同步点 暂时不做拖拽出窗口
	io.Fonts->AddFontFromFileTTF((Pitaya::Core::GetExecutableDirectory() / "resource/fonts/segoeui.ttf").string().c_str(), 16.0f, nullptr, io.Fonts->GetGlyphRangesChineseFull());
	ImFontConfig iconsConfig;
	iconsConfig.MergeMode = true;
	iconsConfig.PixelSnapH = true;
	static const constexpr ImWchar iconsRanges[] = { ICON_MIN_FA, ICON_MAX_16_FA, 0 };
	io.Fonts->AddFontFromFileTTF((Pitaya::Core::GetExecutableDirectory() / "resource/fonts/fa-solid-900.ttf").string().c_str(), 16.0f, &iconsConfig, iconsRanges);

	ImGuiStyle& style = ImGui::GetStyle();
	ImGui::StyleColorsDark();
	style.WindowPadding = ImVec2(8.0f, 8.0f);
	style.FramePadding = ImVec2(6.0f, 3.0f);
	style.ItemSpacing = ImVec2(4.0f, 4.0f);
	style.ItemInnerSpacing = ImVec2(4.0f, 4.0f);

	style.WindowRounding = 0.0f;                   // 主窗口纯直角
	style.FrameRounding = 3.0f;                    // 输入框/按钮带有微微的圆角 (Unity风格)
	style.PopupRounding = 2.0f;
	style.ScrollbarRounding = 9.0f;                // 滚动条极其圆润
	style.GrabRounding = 3.0f;                     // 滑块圆角
	style.TabRounding = 2.0f;                      // Tab标签微微圆角

	style.WindowBorderSize = 1.0f;
	style.FrameBorderSize = 1.0f;
	style.PopupBorderSize = 1.0f;
	style.TabBorderSize = 0.0f;

	// 抗锯齿优化，让界面更平滑
	style.AntiAliasedLines = true;
	style.AntiAliasedFill = true;

	ImVec4 bgColor = ImVec4(0.22f, 0.22f, 0.22f, 1.00f);		// 经典的中灰色
	ImVec4 darkBgColor = ImVec4(0.16f, 0.16f, 0.16f, 1.00f);	// 稍暗的灰色 (背景/未激活标签)
	ImVec4 groupColor = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);		// Inspector 里组件的背景色
	ImVec4 frameColor = ImVec4(0.11f, 0.11f, 0.11f, 1.00f);		// 输入框：深灰色，沉下去
	ImVec4 accentColor = ImVec4(0.24f, 0.52f, 0.88f, 1.00f);
	ImVec4 accentColorHover = ImVec4(0.31f, 0.59f, 0.96f, 1.00f);
	ImVec4 accentColorActive = ImVec4(0.18f, 0.44f, 0.76f, 1.00f);
	ImVec4 textLight = ImVec4(0.85f, 0.85f, 0.85f, 1.00f);
	ImVec4 textDark = ImVec4(0.55f, 0.55f, 0.55f, 1.00f);

	// 【文本】
	style.Colors[ImGuiCol_Text] = textLight;
	style.Colors[ImGuiCol_TextDisabled] = textDark;

	// 【背景】
	style.Colors[ImGuiCol_WindowBg] = bgColor;
	style.Colors[ImGuiCol_ChildBg] = bgColor;              // Hierarchy 等子窗口保持同样的灰
	style.Colors[ImGuiCol_PopupBg] = darkBgColor;

	// 【边框线条 - 极其重要】
	style.Colors[ImGuiCol_Border] = ImVec4(0.13f, 0.13f, 0.13f, 1.00f); // 明显的分割线
	style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);

	// 【Frame (输入框 / Inspector滑条组)】
	style.Colors[ImGuiCol_FrameBg] = frameColor;
	style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f); // 放上去变浅灰
	style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.28f, 0.28f, 0.28f, 1.00f);

	// 【标题栏 (Docking 顶部的可拖拽黑带)】
	style.Colors[ImGuiCol_TitleBg] = darkBgColor;
	style.Colors[ImGuiCol_TitleBgActive] = darkBgColor;
	style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.16f, 0.16f, 0.16f, 0.50f);
	style.Colors[ImGuiCol_MenuBarBg] = darkBgColor;

	// 【Tab标签页】
	style.Colors[ImGuiCol_Tab] = darkBgColor;							// 未选中时，暗色
	style.Colors[ImGuiCol_TabHovered] = groupColor;						// 鼠标悬浮微亮
	style.Colors[ImGuiCol_TabActive] = bgColor;							// 选中时，和平级窗口颜色一致
	style.Colors[ImGuiCol_TabUnfocused] = darkBgColor;
	style.Colors[ImGuiCol_TabUnfocusedActive] = bgColor;

	// 【折叠面板 (Hierarchy的父节点 / Inspector的各大组件框)】
	style.Colors[ImGuiCol_Header] = groupColor; 
	style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.30f, 0.30f, 0.30f, 1.00f);
	style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.35f, 0.35f, 0.35f, 1.00f);

	// 【按钮】
	style.Colors[ImGuiCol_Button] = groupColor;
	style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.35f, 0.35f, 0.35f, 1.00f);
	style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.40f, 0.40f, 0.40f, 1.00f);

	// 【强调色 (光标 / Checkbox勾选 / Slider蓝色区域)】
	style.Colors[ImGuiCol_CheckMark] = accentColor;
	style.Colors[ImGuiCol_SliderGrab] = accentColorHover;
	style.Colors[ImGuiCol_SliderGrabActive] = accentColorActive;
	style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(accentColor.x, accentColor.y, accentColor.z, 0.40f);

	// 【分割线 (Resize拖拽条)】
	style.Colors[ImGuiCol_Separator] = style.Colors[ImGuiCol_Border];		// 分割线与边框同色
	style.Colors[ImGuiCol_SeparatorHovered] = accentColor;                  // 拖拽窗口边缘时亮起
	style.Colors[ImGuiCol_SeparatorActive] = accentColorActive;

	// 【滚动条】
	style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.35f, 0.35f, 0.35f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.45f, 0.45f, 0.45f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.55f, 0.55f, 0.55f, 1.00f);

	// ImGuizmo Style
	ImGuizmo::Style& styleGuizmo = ImGuizmo::GetStyle();

	// 设置线条和箭头的粗细
	styleGuizmo.TranslationLineThickness = 4.0f;   // 默认较细，调大后更像 Unity
	styleGuizmo.TranslationLineArrowSize = 10.0f;  // 箭头大小
	styleGuizmo.RotationLineThickness = 4.0f;   // 旋转环的粗细
	styleGuizmo.RotationOuterLineThickness = 2.0f;
	styleGuizmo.ScaleLineThickness = 4.0f;
	styleGuizmo.ScaleLineCircleSize = 8.0f;   // 缩放末端的方块大小

	// 设置中间那个“中心圆点/方块”的大小
	styleGuizmo.CenterCircleSize = 5.0f;

	// Gizmo 的显示比例 
	ImGuizmo::SetGizmoSizeClipSpace(0.12f);
}
void Pitaya::Editor::GUI::NewFrame()
{
	BeginFrame();
	SetupDockSpace();
	DrawPanels();
	EndFrame();
}
void Pitaya::Editor::GUI::BeginFrame()
{
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame(); 
	ImGuizmo::BeginFrame();
}
void Pitaya::Editor::GUI::SetupDockSpace()
{
	static bool fullscreen = true;
	static ImGuiDockNodeFlags dockspaceFlags =
		ImGuiDockNodeFlags_PassthruCentralNode |
		ImGuiDockNodeFlags_NoWindowMenuButton |
		ImGuiDockNodeFlags_NoCloseButton;
	ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoBringToFrontOnFocus |
		ImGuiWindowFlags_NoNavFocus |
		ImGuiWindowFlags_MenuBar;

	int styleColorPushCount = 0;
	int styleVarPushCount = 0;
	if (fullscreen)
	{
		const ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->WorkPos);
		ImGui::SetNextWindowSize(viewport->WorkSize);
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		styleVarPushCount = 2;
	}
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
	styleColorPushCount = 1;
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	styleVarPushCount += 1;

	ImGui::Begin("DockSpace", nullptr, windowFlags);
	ImGui::PopStyleVar(1);
	styleVarPushCount -= 1;
	if (fullscreen && styleVarPushCount > 0)
	{
		ImGui::PopStyleVar(styleVarPushCount);
		styleVarPushCount = 0;
	}

	DrawMenuBar();
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));
	DrawToolbar();
	ImGui::PopStyleVar();

	ImGuiIO& io = ImGui::GetIO();
	if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
	{
		ImGuiID dockspaceId = ImGui::GetID("DockSpace");
		if (ImGui::DockBuilderGetNode(dockspaceId) == nullptr)
		{
			ImGui::DockBuilderRemoveNode(dockspaceId);
			ImGui::DockBuilderAddNode(dockspaceId, dockspaceFlags | ImGuiDockNodeFlags_DockSpace);
			ImGui::DockBuilderSetNodeSize(dockspaceId, ImGui::GetMainViewport()->WorkSize);
			ImGui::DockBuilderFinish(dockspaceId);
		}
		ImGui::DockSpace(dockspaceId, ImVec2(0.0f, 0.0f), dockspaceFlags);
	}

	if (styleColorPushCount > 0)
	{
		ImGui::PopStyleColor(styleColorPushCount);
		styleColorPushCount = 0;
	}
}
void Pitaya::Editor::GUI::DrawMenuBar()
{
	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("Panel"))
		{
			for (auto& panel : panels.Each())
			{
				if (ImGui::MenuItem(panel->GetName().data(), nullptr, panel->GetOpenState()))
				{
					panel->Toggle();
				}
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("System"))
		{
			static bool isConsoleOpen = (GetConsoleWindow() != nullptr);
			if (ImGui::MenuItem("Console", nullptr, &isConsoleOpen))
			{
				if (isConsoleOpen)
				{
					OpenConsole();
				}
				else
				{
					CloseConsole();
				}
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Utils"))
		{
			if (ImGui::MenuItem("MemoryAnalysis"))
			{
				mi_stats_print(NULL);
				panels.consolePanel.Console(Pitaya::Log::LogLevel::Debug, Pitaya::Core::GetMemoryState());
			}
			if (ImGui::MenuItem("HookState"))
			{
				panels.consolePanel.Console(Pitaya::Log::LogLevel::Debug, HOOK_STATE);
			}
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}
}
void Pitaya::Editor::GUI::DrawToolbar()
{
	ImGuiStyle& style = ImGui::GetStyle();
	ImVec4 toolbarBgColor = style.Colors[ImGuiCol_MenuBarBg];

	ImGui::PushStyleColor(ImGuiCol_ChildBg, toolbarBgColor);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

	float toolbarHeight = 24.0f;
	ImGui::BeginChild("##EditorToolbar", ImVec2(0, toolbarHeight), false, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

	ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 3.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2.0f, 2.0f));	//强制缩小内部边距 使16px的图标在20x20的按钮中完美居中(16 + 2x2 = 20)
	ImGui::PushStyleColor(ImGuiCol_Border, style.Colors[ImGuiCol_BorderShadow]);

	//布局基础参数 
	float buttonSize = 20.0f;
	ImVec2 btnSize(buttonSize, buttonSize);
	float centerY = (toolbarHeight - buttonSize) * 0.5f; //Y轴动态居中
	float sidePadding = 8.0f;
	float windowWidth = ImGui::GetWindowWidth();	//获取整个工具栏真正的总宽度

	ImVec4 bgNormal = style.Colors[ImGuiCol_Button];
	ImVec4 bgHover = style.Colors[ImGuiCol_ButtonHovered];
	ImVec4 bgActiveClick = style.Colors[ImGuiCol_ButtonActive];
	ImVec4 toolActiveBg = ImVec4(0.2f, 0.5f, 0.8f, 1.0f);
	ImVec4 toolHoverBg = ImVec4(0.3f, 0.6f, 0.9f, 1.0f);

	//左侧区域 
	ImGui::SetCursorPosY(centerY);		//分开设置 X 和 Y 可以防止 SameLine 导致的高度扰动
	ImGui::SetCursorPosX(sidePadding);
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(2.0f, 0.0f));

	const char* toolNames[] = {
		ICON_FA_ARROW_POINTER,
		ICON_FA_ARROWS_UP_DOWN_LEFT_RIGHT,
		ICON_FA_ROTATE,
		ICON_FA_EXPAND };
	const char* toolTips[] = { "Select", "Translate", "Rotate", "Scale" };
	TransformTool tools[] = { TransformTool::Select, TransformTool::Translate, TransformTool::Rotate, TransformTool::Scale };

	for (int i = 0; i < 4; ++i)
	{
		bool isToolActive = (context.ToolState.ActiveTool == tools[i]);
		ImGui::PushStyleColor(ImGuiCol_Button, isToolActive ? toolActiveBg : bgNormal);
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, isToolActive ? toolHoverBg : bgHover);
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, isToolActive ? toolActiveBg : bgActiveClick);

		if (ImGui::Button(toolNames[i], btnSize)) { context.ToolState.ActiveTool = tools[i]; }
		if (ImGui::IsItemHovered()) { ImGui::SetTooltip("%s", toolTips[i]); }

		ImGui::PopStyleColor(3);
		if (i < 3) { ImGui::SameLine(); } //内部四个按钮需要SameLine
	}
	ImGui::PopStyleVar();

	//中间区域 
	float spacing = 6.0f;
	float centerGroupWidth = (buttonSize * 3.0f) + (spacing * 2.0f); //精确计算3个正方形按钮加间距的总宽度 (3 * 20) + (2 * 6) = 72
	float centerX = (windowWidth - centerGroupWidth) * 0.5f; //居中公式 = (屏幕总宽 - 组总宽) / 2

	ImGui::SetCursorPosY(centerY);
	ImGui::SetCursorPosX(centerX);
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(spacing, 0.0f));

	//引擎当前状态 TODO应该从Editor类去拿
	static EngineState currentState = EngineState::Edit;

	//Play / Stop 按钮
	bool isPlaying = (currentState == EngineState::Play);
	ImGui::PushStyleColor(ImGuiCol_Button, isPlaying ? toolActiveBg : bgNormal);
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, isPlaying ? toolHoverBg : bgHover);
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, isPlaying ? toolActiveBg : bgActiveClick);

	if (ImGui::Button(isPlaying ? ICON_FA_STOP : ICON_FA_PLAY, btnSize))
	{
		currentState = (currentState == EngineState::Edit) ? EngineState::Play : EngineState::Edit;
		context.ToolState.IsPaused = false;
	}
	if (ImGui::IsItemHovered()) { ImGui::SetTooltip(isPlaying ? "Stop" : "Play"); }
	ImGui::PopStyleColor(3);
	ImGui::SameLine();

	//禁用态逻辑 (未播放时不可暂停和单步)
	bool canPauseOrStep = (currentState == EngineState::Play);
	if (!canPauseOrStep) { ImGui::BeginDisabled(); }

	//Pause 按钮
	ImGui::PushStyleColor(ImGuiCol_Button, context.ToolState.IsPaused ? toolActiveBg : bgNormal);
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, context.ToolState.IsPaused ? toolHoverBg : bgHover);
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, context.ToolState.IsPaused ? toolActiveBg : bgActiveClick);
	if (ImGui::Button(ICON_FA_PAUSE, btnSize)) { context.ToolState.IsPaused = !context.ToolState.IsPaused; }
	if (ImGui::IsItemHovered()) { ImGui::SetTooltip("Pause"); }
	ImGui::PopStyleColor(3);
	ImGui::SameLine();

	//Step 按钮
	ImGui::PushStyleColor(ImGuiCol_Button, bgNormal);
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, bgHover);
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, bgActiveClick);
	if (ImGui::Button(ICON_FA_FORWARD_STEP, btnSize))
	{
		// TODO Step 逻辑
	}
	if (ImGui::IsItemHovered()) { ImGui::SetTooltip("Step Frame"); }
	ImGui::PopStyleColor(3);

	if (!canPauseOrStep) { ImGui::EndDisabled(); }

	ImGui::PopStyleVar(); //弹出中部的间距

	//右侧区域
	float rightBtnWidth = 66.0f;

	//核心定位：总宽度 - 按钮本身宽度 - 要求的右边距
	float rightX = windowWidth - rightBtnWidth - sidePadding;

	ImGui::SetCursorPosY(centerY);
	ImGui::SetCursorPosX(rightX);
	if (ImGui::Button(context.ToolState.IsLocal ? ICON_FA_CUBE " Local" : ICON_FA_GLOBE " Global", ImVec2(rightBtnWidth, buttonSize))) { context.ToolState.IsLocal = !context.ToolState.IsLocal; }
	if (ImGui::IsItemHovered()) { ImGui::SetTooltip("Toggle Gizmo Coordinate Space"); }

	ImGui::PopStyleColor(); //ImGuiCol_Border
	ImGui::PopStyleVar(3);

	ImGui::EndChild();

	//绘制底端分割线
	ImVec2 rectMin = ImGui::GetItemRectMin(); //左上角坐标
	ImVec2 rectMax = ImGui::GetItemRectMax(); //右下角坐标
	ImGui::GetWindowDrawList()->AddLine(
		ImVec2(rectMin.x, rectMax.y - 1.0f),
		ImVec2(rectMax.x, rectMax.y - 1.0f),
		ImGui::ColorConvertFloat4ToU32(style.Colors[ImGuiCol_Border]),
		1.0f);	//1像素粗细

	ImGui::PopStyleVar();	//背景Padding
	ImGui::PopStyleColor(); //ChildBg颜色
}
void Pitaya::Editor::GUI::DrawPanels()
{
	for (auto& panel : panels.Each())
	{
		panel->Draw();
	}
}
void Pitaya::Editor::GUI::EndFrame()
{
	ImGui::End();
	ImGui::Render();

	//TODO  涉及到同步点 暂时不做拖拽出窗口
	//if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	//{
	//	ImGui::UpdatePlatformWindows();
	//	ImGui::RenderPlatformWindowsDefault();
	//	glfwMakeContextCurrent(nullptr);
	//}
}

void Pitaya::Editor::GUI::Drawer::Draw(Pitaya::Core::PassKey<Editor>)
{
#if PITAYA_VERSION >= 100
	//[VERSION >= 100] 通过预处理实现Hanlde映射Texture
	//[VERSION  < 100] 通过修改ImGui_ImplOpenGL3_RenderDrawData源码实现Hanlde映射Texture
	//for (uint32_t i = 0; i < buffer[backBufferIndex].CmdListsCount; i++)
	//{
	//	ImDrawList* cmdList = buffer[backBufferIndex].CmdLists[i];
	//	for (uint32_t j = 0; j < cmdList->CmdBuffer.Size; j++)
	//	{
	//		ImDrawCmd* pcmd = &cmdList->CmdBuffer[j];
	//		if ((pcmd->UserCallback != nullptr) || (pcmd->TexRef._TexData != nullptr) ||
	//			(pcmd->TexRef._TexID == 0)) {
	//			continue;
	//		}

	//		uint32_t handleId = (uint32_t)(intptr_t)pcmd->TexRef._TexID;
	//		GLuint realGLTextureID = handleId; // TODO Hanle映射真实textureid
	//		pcmd->TexRef._TexID = (ImTextureID)(intptr_t)realGLTextureID;
	//	}
	//}
#endif
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	int width, height;
	glfwGetFramebufferSize(static_cast<GLFWwindow*>(nativeWindow), &width, &height);
	glViewport(0, 0, width, height);
	glClearColor(Pitaya::Core::Color::Dark.r, Pitaya::Core::Color::Dark.g, Pitaya::Core::Color::Dark.b, Pitaya::Core::Color::Dark.a);
	glClearDepth(1.0f);
	glClearStencil(0x00);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplOpenGL3_RenderDrawData(&buffer[backBufferIndex]);
}
