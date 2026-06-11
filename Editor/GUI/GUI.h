#pragma once

#include<Core/PassKey/PassKey.h>
#include<Window/Common/FuncTable.h>
#include<Editor/GUI/Panel/HierarchyPanel.h>
#include<Editor/GUI/Panel/SceneViewportPanel.h>
#include<Editor/GUI/Panel/GameViewportPanel.h>
#include<Editor/GUI/Panel/InspectorPanel.h>
#include<Editor/GUI/Panel/ConsolePanel.h>
#include<Editor/GUI/Panel/ProfilerPanel.h>
#include<Editor/GUI//Panel/PreferencesPanel.h>
#include<Editor/GUI/Panel/ProjectPanel.h>
#include<Editor/GUI/Panel/UtilsPanel.h>
#include<Editor/Common/TransformTool.h>

#include<string>
#include<atomic>
#include<array>
#include<glm.hpp>

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
			struct Buffer
			{
				ImDrawData DrawData;
				glm::uvec2 WindowSize;
			};

		private:
			Drawer() = default;
			~Drawer() = default;

		public:
			Drawer(const Drawer&) = delete;
			Drawer& operator=(const Drawer&) = delete;
			Drawer(Drawer&&) = delete;
			Drawer& operator=(Drawer&&) = delete;

		private:
			inline bool Initialize(void* nativeWindow)
			{
				this->nativeWindow = nativeWindow;
				glm::uvec2 windowSize = Pitaya::Window::GetWindowSize();
				buffers[0].WindowSize = windowSize;
				buffers[1].WindowSize = windowSize;
				return true;
			}
			inline void Release()
			{
				ReleaseDrawData(0);
				ReleaseDrawData(1);
				this->nativeWindow = nullptr;
			}

		public:
			void Draw(Pitaya::Core::PassKey<Editor>);	//MOUNT_POSTRENDERERPARSECOMMAND_HOOK（第一步）

		public:
			inline void SwapBuffer(Pitaya::Core::PassKey<Editor>) noexcept	//MOUNT_POSTRENDERERSWAPBUFFER_HOOK
			{
				writeIndex = 1 - writeIndex;
			}
			inline void CreateFrontDrawData(Pitaya::Core::PassKey<Editor>)	//MOUNT_PRERENDERERENDRENDERFRAME_HOOK
			{
				CreateDrawData(writeIndex);
			}
			inline void ReleaseFrontDrawData(Pitaya::Core::PassKey<Editor>)	//MOUNT_POSTRENDERERBEGINRENDERFRAME_HOOK
			{
				ReleaseDrawData(writeIndex);
			}
			inline void ReleaseBackDrawData(Pitaya::Core::PassKey<Editor>)	//MOUNT_POSTRENDERERPARSECOMMAND_HOOK（在Draw绘制后执行）
			{
				ReleaseDrawData(1 - writeIndex);
			}
			inline bool HasRenderDrawData(Pitaya::Core::PassKey<Editor>)	//MOUNT_SHOULDWAKEUPRENDERTHREAD_HOOK
			{
				return buffers[1 - writeIndex].DrawData.CmdLists.Size > 0;
			}

		private:
			inline void CreateDrawData(uint32_t index)
			{
				ReleaseDrawData(index);
				ImDrawData* src = ImGui::GetDrawData();
				if (!src || src->CmdListsCount == 0 || src->TotalVtxCount == 0) { return; }
				buffers[index].DrawData = *src;
				buffers[index].DrawData.CmdLists.clear();
				buffers[index].DrawData.CmdLists.reserve(src->CmdListsCount);
				for (uint32_t i = 0; i < src->CmdListsCount; i++)
				{
					ImDrawList* srcList = src->CmdLists[i];
					ImDrawList* dstList = srcList->CloneOutput();
					buffers[index].DrawData.CmdLists.push_back(dstList);
				}
				buffers[index].WindowSize = Pitaya::Window::GetWindowSize();
			}
			inline void ReleaseDrawData(uint32_t index)
			{
				for (uint32_t i = 0; i < buffers[index].DrawData.CmdLists.Size; i++)
				{
					IM_DELETE(buffers[index].DrawData.CmdLists[i]);
				}
				buffers[index].DrawData.CmdLists.clear();
			}

		private:
			std::array<Buffer, 2> buffers;	// 双缓冲 [writeIndex] = 主线程写入, [1-writeIndex] = 渲染线程读取
			uint32_t writeIndex = 0;		// 主线程写入
			void* nativeWindow = nullptr;
		};
		class Panels
		{
			friend class Pitaya::Editor::GUI;
			friend class Pitaya::Editor::Editor;
		private:
			Panels() = default;
			~Panels() = default;

		public:
			Panels(const Panels&) = delete;
			Panels& operator=(const Panels&) = delete;
			Panels(Panels&&) = delete;
			Panels& operator=(Panels&&) = delete;

		public:
			inline constexpr auto Each() noexcept
			{
				return std::to_array<Panel*>({ &hierarchyPanel, &inspectorPanel, &profilerPanel, &preferencesPanel,
									&projectPanel, &sceneViewportPanel, &gameViewportPanel, &consolePanel, &utilsPanel });
			}

		private:
			HierarchyPanel hierarchyPanel;
			InspectorPanel inspectorPanel;
			ConsolePanel consolePanel;
			ProfilerPanel profilerPanel;
			PreferencesPanel preferencesPanel;
			ProjectPanel projectPanel;
			SceneViewportPanel sceneViewportPanel;
			GameViewportPanel gameViewportPanel;
			UtilsPanel utilsPanel;
		};

	public:
		struct Context
		{
			struct ToolState
			{
				TransformTool ActiveTool = TransformTool::Translate;
				bool IsPaused = false;
				bool IsLocal = true;
			};
			struct GizmoState
			{
				bool ShowCameraGizmo = true;  // 摄像机图标
				bool ShowLightGizmo = true;   // 灯光图标
				bool ShowGrid = true;         // 地面网格
				bool ShowColliders = false;   // 碰撞体框线 (可选的扩充)
			};
			struct Selection
			{
				enum class Type : uint8_t
				{
					Entity = 0,
					File
				};

				entt::entity SelectedEntity = entt::null;       //选中实体时的数据
				Type Type = Type::Entity;
			};

			ToolState ToolState;
			GizmoState GizmoState;
			Selection Selection;
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
		bool Initialize_Main(void* nativeWindow);
		bool Initialize_Render(Pitaya::Core::SlotMap<Pitaya::GPU::FrameBuffer>::Handle gameRtHandle, Pitaya::Core::SlotMap<Pitaya::GPU::FrameBuffer>::Handle editorRtHandle);
		void Release_Main();
		void Release_Render();

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

	public:
		inline Pitaya::Editor::GUI::Context& GetContext() noexcept
		{
			return context;
		}
	
	private:
		Pitaya::Editor::GUI::Drawer drawer;
		Pitaya::Editor::GUI::Panels panels;
		Pitaya::Editor::GUI::Context context;

		std::atomic<ImGuiContext*> imGuiContext = nullptr;
		std::atomic<bool> isRenderReady = false;
	};
}
