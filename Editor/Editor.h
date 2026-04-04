#pragma once

#include<Core/PassKey/PassKey.h>
#include<Core/Allocate/Allocate.h>
#include<Context/Common/Module.h>
#include<Editor/Camera/Camera.h>
#include<Editor/GUI/GUI.h>
#include<Editor/Common/EngineState.h>
#include<Event/Common/Event.h>
#include<Log/Common/LogLevel.h>

namespace Pitaya::Editor
{
	class Editor
	{
	public:
		struct Factory
		{
			friend class Pitaya::Engine::Module<Editor>;
		private:
			static Editor* Create()
			{
				return PITAYA_NEW(Editor);
			}
			static void Destroy(Editor* editor)
			{
				PITAYA_DELETE(editor);
			}
		};
		struct Lifecycle
		{
			friend class Pitaya::Engine::Module<Editor>;
		private:
			static bool Initialize(Editor* editor, void* nativeWindow)
			{
				return editor->Initialize(nativeWindow);
			}
			static void Release(Editor* editor)
			{
				editor->Release();
			}
			static void Update(Editor* editor)
			{
				editor->Updata();
			}
		};

	private:
		Editor() = default;
		~Editor() = default;

	public:
		Editor(const Editor&) = delete;
		Editor& operator=(const Editor&) = delete;
		Editor(Editor&&) = delete;
		Editor& operator=(Editor&&) = delete;

	private:
		bool Initialize(void* nativeWindow);
		void Release();

	public:
		bool InitializeForRender(Pitaya::Core::PassKey<Pitaya::Render::Renderer>);
		void ReleaseForRender(Pitaya::Core::PassKey<Pitaya::Render::Renderer>);

	public:
		void Console(Pitaya::Log::LogLevel level, std::string_view message);

	private:
		void Updata();

	public:
		inline void GUINewFrame(Pitaya::Core::PassKey<Pitaya::Engine::Engine>)
		{
			gui.NewFrame();
		}
		
	public:
		inline bool GetGUIScenePanelVisablew() const noexcept
		{
			return gui.sceneViewportPanel.GetIsVisable();
		}

		inline Pitaya::Editor::Camera& GetCamera() noexcept
		{
			return camera;
		}

	public:
		inline Pitaya::Editor::EngineState GetEngineState() const noexcept
		{
			return engineState;
		}

	private:
		void OnMouseScroll(const Pitaya::Event::Event&);
		void OnMouseCurrsorMove(const Pitaya::Event::Event&);

	private:
		inline static void OnMouseScroll(void* listener, const Pitaya::Event::Event& event)
		{
			static_cast<Pitaya::Editor::Editor*>(listener)->OnMouseScroll(event);
		}
		inline static void OnMouseCurrsorMove(void* listener, const Pitaya::Event::Event& event)
		{
			static_cast<Pitaya::Editor::Editor*>(listener)->OnMouseCurrsorMove(event);
		}

	private:
		Pitaya::Editor::GUI gui;
		Pitaya::Editor::Camera camera;
		Pitaya::Editor::EngineState engineState = EngineState::Edit;

		Pitaya::Event::EventToken mouseScrollToken;
		Pitaya::Event::EventToken mouseCurrsorMoveToken;
	};
}
