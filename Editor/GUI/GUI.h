#pragma once

#include<Core/PassKey/PassKey.h>

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
		class Drawer
		{
			friend class Pitaya::Editor::GUI;
		private:
			Drawer() = default;
			~Drawer() = default;

		public:
			Drawer(const Drawer&) = delete;
			Drawer& operator=(const Drawer&) = delete;
			Drawer(Drawer&&) = delete;
			Drawer& operator=(Drawer&&) = delete;

		private:
			inline bool Initialize(void* nativeWindow) noexcept
			{
				this->nativeWindow = nativeWindow;
				return true;
			}
			inline void Release()
			{
				ReleaseDrawData(frontBufferIndex);
				ReleaseDrawData(backBufferIndex);
			}

		public:
			void Draw(Pitaya::Core::PassKey<Editor>);	//MOUNT_POSTRENDERERPARSECOMMAND_HOOK（第一步）

		public:
			inline void SwapBuffer(Pitaya::Core::PassKey<Editor>) noexcept	//MOUNT_POSTRENDERERSWAPBUFFER_HOOK
			{
				std::swap(frontBufferIndex, backBufferIndex);
			}
			inline void CreateFrontDrawData(Pitaya::Core::PassKey<Editor>)	//MOUNT_PRERENDERERENDRENDERFRAME_HOOK
			{
				CreateDrawData(frontBufferIndex);
			}
			inline void ReleaseFrontDrawData(Pitaya::Core::PassKey<Editor>)	//MOUNT_POSTRENDERERBEGINRENDERFRAME_HOOK
			{
				ReleaseDrawData(frontBufferIndex);
			}
			inline void ReleaseBackDrawData(Pitaya::Core::PassKey<Editor>)	//MOUNT_POSTRENDERERPARSECOMMAND_HOOK（在Draw绘制后执行）
			{
				ReleaseDrawData(backBufferIndex);
			}
			inline bool HasRenderDrawData(Pitaya::Core::PassKey<Editor>)	//MOUNT_SHOULDWAKEUPRENDERTHREAD_HOOK
			{
				return buffer[backBufferIndex].CmdLists.Size > 0;
			}

		private:
			inline void CreateDrawData(uint8_t index)
			{
				ReleaseDrawData(index);
				ImDrawData* src = ImGui::GetDrawData();
				if (!src || src->CmdListsCount == 0 || src->TotalVtxCount == 0) { return; }
				buffer[index] = *src;
				buffer[index].CmdLists.clear();
				buffer[index].CmdLists.reserve(src->CmdListsCount);
				for (uint32_t i = 0; i < src->CmdListsCount; i++)
				{
					ImDrawList* srcList = src->CmdLists[i];
					ImDrawList* dstList = srcList->CloneOutput();
					buffer[index].CmdLists.push_back(dstList);
				}
			}
			inline void ReleaseDrawData(uint8_t index)
			{
				for (uint32_t i = 0; i < buffer[index].CmdLists.Size; i++)
				{
					IM_DELETE(buffer[index].CmdLists[i]);
				}
				buffer[index].CmdLists.clear();
			}

		private:
			ImDrawData buffer[2] = {};
			uint8_t frontBufferIndex = 0;	//主线程写入
			uint8_t backBufferIndex = 1;	//渲染线程读取
			void* nativeWindow = nullptr;
		};

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
		Drawer drawer;

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
