#pragma once

#include<Core/PassKey/PassKey.h>
#include<Editor/API/def.h>
#include<Editor/Camera/Camera.h>
#include<Editor/GUI/GUI.h>
#include<Editor/Profiler/Profiler.h>

#include<Event/Common/Event.h>
#include<Log/Common/LogLevel.h>

namespace Pitaya::Editor
{
	class Editor : public Pitaya::Core::Singleton<Editor>
	{
		friend class Pitaya::Core::Singleton<Editor>;
	private:
		Editor() = default;
		~Editor() = default;

	public:
		Editor(const Editor&) = delete;
		Editor& operator=(const Editor&) = delete;
		Editor(Editor&&) = delete;
		Editor& operator=(Editor&&) = delete;

	public:
		void MountEngineHook();

	private:
		bool Initialize(void* nativeWindow);
		void Release();

	public:
		bool InitializeForRender();
		void ReleaseForRender();

	private:
		void BeginFrame();
		void FixedUpdate();
		void Updata();
		void LateUpdate();
		void EndFrame();
			
	public:
		inline Pitaya::Editor::Camera& GetCamera() noexcept
		{
			return camera;
		}
		inline Pitaya::Editor::Profiler& GetProfiler() noexcept
		{
			return profiler;
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
		Pitaya::Editor::Profiler profiler;

		Pitaya::Event::EventToken mouseScrollToken;
		Pitaya::Event::EventToken mouseCurrsorMoveToken;
	};
}


template<>
EDITOR_API Pitaya::Editor::Editor& EDITOR_CALL Pitaya::Core::Singleton<Pitaya::Editor::Editor>::Instance();
