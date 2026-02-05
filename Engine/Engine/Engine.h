#pragma once

#include<Enum/Enum.h>
#include<Singleton/Singleton.h>

#include<Engine/Config/Config.h>

namespace Pitaya::Engine::Interface
{
	class Renderer;
	class Physics;
	class Window;
}

namespace Pitaya::Engine::Internal
{
	class Input;
	class Time;
	class Event;
	class Log;
	class Thread;
}

namespace Pitaya::Engine
{
	class Engine : public Pitaya::Core::Singleton<Engine>
	{
		friend class Pitaya::Core::Singleton<Pitaya::Engine::Engine>;
	private:
		Engine() = default;
		~Engine() = default;

	public:
		Engine(const Engine&) = delete;
		Engine& operator=(const Engine&) = delete;
		Engine(Engine&&) = delete;
		Engine& operator=(Engine&&) = delete;

	public:
		inline int Execute()
		{
			if (!Initialize())
			{
				return -1;
			}

			while (IsRunning())
			{
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

		inline Pitaya::Engine::Internal::Input* GetInputModel() const noexcept
		{
			return input;
		}
		inline Pitaya::Engine::Internal::Time* GetTimeModel() const noexcept
		{
			return time;
		}
		inline Pitaya::Engine::Internal::Event* GetEventModel() const noexcept
		{
			return event;
		}
		inline Pitaya::Engine::Internal::Log* GetLogModel() const noexcept
		{
			return log;
		}
		inline Pitaya::Engine::Internal::Thread* GetThreadModel() const noexcept
		{
			return thread;
		}

		inline Pitaya::Engine::Config::Config& GetConfigModel() noexcept
		{
			return config;
		}

	private:
		bool Initialize();
		bool IsRunning();
		void Tick();
		void Fixupdata();
		void Updata();
		void CoroutineSchedule();
		void Lateupdata();
		void Renderer();
		void EndOfFrame();
		void FrameSync();
		void Release();

	private:
		bool CreateEngineModel();
		bool CheckEngineModel();

	private:
		Pitaya::Engine::Interface::Renderer* renderer = nullptr;
		Pitaya::Engine::Interface::Physics* physics = nullptr;
		Pitaya::Engine::Interface::Window* window = nullptr;

		Pitaya::Engine::Internal::Input* input = nullptr;
		Pitaya::Engine::Internal::Time* time = nullptr;
		Pitaya::Engine::Internal::Event* event = nullptr;
		Pitaya::Engine::Internal::Log* log = nullptr;
		Pitaya::Engine::Internal::Thread* thread = nullptr;
		//coroutine
		//game
		//camera

		Pitaya::Engine::Config::Config config;
	};
}