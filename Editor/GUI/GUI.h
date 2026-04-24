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
				size[0] = windowSize;
				size[1] = windowSize;
				return true;
			}
			inline void Release()
			{
				ReleaseDrawData(frontBufferIndex);
				ReleaseDrawData(backBufferIndex);
				this->nativeWindow = nullptr;
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
				size[index] = Pitaya::Window::GetWindowSize();
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
			glm::uvec2 size[2] = {};		//窗口尺寸
			uint8_t frontBufferIndex = 0;	//主线程写入
			uint8_t backBufferIndex = 1;	//渲染线程读取
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
									&projectPanel, &sceneViewportPanel, &gameViewportPanel, &consolePanel });
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
		bool Initialize_Render(uint64_t gameRT, uint64_t editorRT);
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
