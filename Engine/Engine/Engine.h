#pragma once

#include"Define/Define.h"
#include"Enum/Enum.h"

#include"Interface/Renderer.h"
#include"Interface/Physics.h"
#include"Interface/Window.h"

#include"Internal/Input/Input.h"
#include"Internal/Time/Time.h"

namespace Pitaya::Engine
{
	class Engine
	{
		DECLARE_SINGLETON_CLASS_RI(Engine);

	public:
		inline int Execute()
		{
			if (!Initialize())
			{
				return -1;
			}

			while (IsRunning())
			{
				Input();
				Tick();
				Fixupdata();
				Updata();
				CoroutineSchedule();
				Lateupdata();
				Renderer();
				EndOfFrame();
				FrameSync();
			}

			Release();
			return 0;
		}

#pragma region Input
		inline bool GetKeyDown(Pitaya::Engine::Input::KeyCode keyCode) const
		{
			return window->GetKeyDown(keyCode);
		}
#pragma endregion

#pragma region Time
		inline const float& GetDeltaTime() const
		{
			return time.delta;
		}
		inline const float& GetFixdeltaTime() const
		{
			return time.fixdelta;
		}
		inline const float& GetUnscaledDeltaTime() const
		{
			return time.unscaledDeltaTime;
		}
		inline float& GetTimeScale()
		{
			return time.scale;
		}
		inline float GetSecondsTime()
		{
			return time.Seconds();
		}
		inline int64_t GetMillisecondsTime()
		{
			return time.Milliseconds();
		}
#pragma endregion

	private:
		bool IsRunning();
		void Input();
		void Tick();
		void Fixupdata();
		void Updata();
		void CoroutineSchedule();
		void Lateupdata();
		void Renderer();
		void EndOfFrame();
		void FrameSync();

	private:
		bool BindEngineModelBackendAPI();
		bool CheckEngineModelBackendBindIsSuccess();

	private:
		Pitaya::Engine::Interface::Renderer* renderer = nullptr;
		Pitaya::Engine::Interface::Physics* physics = nullptr;
		Pitaya::Engine::Interface::Window* window = nullptr;

		Pitaya::Engine::Internal::Input input;
		Pitaya::Engine::Internal::Time time;
		//coroutine
		//game
		//camera

		Pitaya::Engine::Renderer::RendererAPI rendererAPI = Pitaya::Engine::Renderer::RendererAPI::OpenGL;
		Pitaya::Engine::Physics::PhysicsAPI physicsAPI = Pitaya::Engine::Physics::PhysicsAPI::Bullet;
	};
}