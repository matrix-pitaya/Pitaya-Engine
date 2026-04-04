#pragma once

#include<Editor/GUI/Panel/HierarchyPanel.h>
#include<Editor/GUI/Panel/ViewportPanel.h>
#include<Editor/GUI/Panel/InspectorPanel.h>
#include<Editor/GUI/Panel/ConsolePanel.h>
#include<Editor/GUI/Panel/ProfilerPanel.h>
#include<Editor/GUI//Panel/PreferencesPanel.h>
#include<Editor/GUI/Panel/ProjectPanel.h>

#include<string>
#include<vector>
#include<atomic>

namespace Pitaya::Editor
{
	class Editor;
	class GUI
	{
		friend class Pitaya::Editor::Editor;
	private:
		GUI() = default;
		~GUI() = default;

	public:
		GUI(const GUI&) = delete;
		GUI& operator=(const GUI&) = delete;
		GUI(GUI&&) = delete;
		GUI& operator=(GUI&&) = delete;

	private:
		bool InitializeForMain(void* nativeWindow);
		void ReleaseForMain();

	private:
		bool InitializeForRender();
		void ReleaseForRender();

	private:
		void SetStyle();

	private:
		void NewFrame();

	private:
		void BeginFrame();
		void SetupDockSpace();
		void DrawMenuBar();
		void DrawToolbar();
		void DrawPanels();
		void EndFrame();
		
	private:
		inline void InitializePanel()
		{
			for (auto panel : panels)
			{
				panel->Initialize();
			}

			//设置回调函数
			hierarchyPanel.SetSelectionChangedCallback(
				[this](int32_t selectedID) {inspectorPanel.SetSelectedEntity(selectedID); });
		}
		inline void ReleasePanel()
		{
			for (auto panel : panels)
			{
				panel->Release();
			}
		}

	private:
		HierarchyPanel hierarchyPanel;
		InspectorPanel inspectorPanel;
		ConsolePanel consolePanel;
		ProfilerPanel profilerPanel;
		PreferencesPanel preferencesPanel;
		ProjectPanel projectPanel;
		ViewportPanel sceneViewportPanel { "Scene" };
		ViewportPanel gameViewportPanel { "Game" };
		std::vector<Panel*> panels = { &hierarchyPanel,&inspectorPanel, &profilerPanel,&preferencesPanel, &projectPanel, &sceneViewportPanel,&gameViewportPanel,&consolePanel };

		std::atomic<ImGuiContext*> imGuiContext = nullptr;
		std::atomic<bool> isRenderReady = false;
	};
}
