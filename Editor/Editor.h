#pragma once

#include<Core/PassKey/PassKey.h>
#include<Editor/API/def.h>
#include<Editor/Camera/Camera.h>
#include<Editor/GUI/GUI.h>
#include<Editor/Profiler/Profiler.h>
#include<Editor/StateMachine/StateMachine.h>
#include<Event/Common/Event.h>
#include<Log/Common/LogLevel.h>

namespace Pitaya::Editor
{
	class Editor : public Pitaya::Core::Singleton<Editor>
	{
		friend class Pitaya::Core::Singleton<Editor>;
	private:
		struct HookFunc
		{
			static void PreBeginFrame();
			static void PreFixedUpdate();
			static void PreUpdate();
			static void PreLateUpdate();
			static void PreEndFrame();
			static void PostRendererIntialize(void*);
			static void PostRendererRelease();
			static void PostRenderContextInitialized();
			static void PreRenderContextReleased();
			static void PostRendererSwapBuffer();
			static void PreRendererEndRenderFrame();
			static void PostRendererBeginRenderFrame();
			static bool ShouldWakeupRenderThread();
			static bool ShouldSubmitSceneCameraPass();
			static void PreRenderPipelineExecute(Pitaya::Core::PassKey<Pitaya::Engine::Engine>, Pitaya::Render::RenderPipeline*);
			static void PostRendererParseCommand();
			static void PostChronometerTick();
			static void PostLog(Pitaya::Log::LogLevel, std::string_view);
			static bool TerminateFixedUpdate();
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
		bool Initialize_Main(void* nativeWindow);
		void Release_Main();
		bool Initialize_Render();
		void Release_Render();

	private:
		void BeginFrame();
		void FixedUpdate();
		void Updata();
		void LateUpdate();
		void EndFrame();
			
	public:
		inline GUI& GetGUI() noexcept
		{
			return gui;
		}
		inline Camera& GetCamera() noexcept
		{
			return camera;
		}
		inline Profiler& GetProfiler() noexcept
		{
			return profiler;
		}
		inline StateMachine& GetStateMachine() noexcept
		{
			return stateMachine;
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

	public:
		static void AttachRuntimeEnv();

	private:
		Pitaya::Editor::GUI gui;
		Pitaya::Editor::Camera camera;
		Pitaya::Editor::Profiler profiler;
		Pitaya::Editor::StateMachine stateMachine;

		Pitaya::Event::EventToken mouseScrollToken;
		Pitaya::Event::EventToken mouseCurrsorMoveToken;
	};
}

template<>
EDITOR_API Pitaya::Editor::Editor& EDITOR_CALL Pitaya::Core::Singleton<Pitaya::Editor::Editor>::Instance();
