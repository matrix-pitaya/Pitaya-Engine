#pragma once

#include"Enum/Enum.h"
#include"Singleton/Singleton.h"

#include"Interface/Renderer.h"
#include"Interface/Physics.h"
#include"Interface/Window.h"

#include"Internal/Input/Input.h"
#include"Internal/Time/Time.h"
#include"Internal/Log/Log.h"
#include"Internal/Event/Event.h"
#include"Internal/Thread/Thread.h"

#include"Config/Config.h"

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

#pragma region Input
		
#pragma endregion

#pragma region Window

#pragma endregion

#pragma region Time
		inline const float& GetDeltaTime() const noexcept
		{
			return time.delta;
		}
		inline const float& GetFixdeltaTime() const noexcept
		{
			return time.fixdelta;
		}
		inline const float& GetUnscaledDeltaTime() const noexcept
		{
			return time.unscaledDeltaTime;
		}
		inline float& GetTimeScale() noexcept
		{
			return time.scale;
		}
		inline float GetSecondsTime() noexcept
		{
			return time.Seconds();
		}
		inline int64_t GetMillisecondsTime() noexcept
		{
			return time.Milliseconds();
		}
#pragma endregion

#pragma region Config
		inline size_t GetMaxFixupdataExecuteTimes() const noexcept
		{
			return config.MaxFixupdateExecuteTimes;
		}
#pragma endregion

#pragma region Log
		inline void LogInfo(const std::string& message) noexcept
		{
			log.LogInfo(message);
		}
		inline void LogDebug(const std::string& message) noexcept
		{
			log.LogDebug(message);
		}
		inline void LogWarning(const std::string& message) noexcept
		{
			log.LogWarning(message);
		}
		inline void LogError(const std::string& message) noexcept 
		{
			log.LogError(message);
		}
#pragma endregion

#pragma region Event
		inline Pitaya::Engine::Event::EventToken SubscribeEvent(::Pitaya::Engine::Event::EventType type, std::function<void(const ::Pitaya::Engine::Event::Event&)> function) noexcept
		{
			return event.Subscribe(type, std::move(function));
		}
		inline bool UnSubscribeEvent(const ::Pitaya::Engine::Event::EventToken& eventToken) noexcept
		{
			return event.UnSubscribe(eventToken);
		}
		inline void EmitEvent(const ::Pitaya::Engine::Event::Event& event) noexcept
		{
			this->event.Emit(event);
		}
#pragma endregion

#pragma region Thread
		template<class Function, class... Args>
		inline Pitaya::Engine::Thread::ThreadToken RegisterThread(const std::string& name, Function&& f, Args&&... args) noexcept 
		{
			log.LogDebug(name + "线程被注册!");
			return thread.RegisterThread(name, std::forward<Function>(f), std::forward<Args>(args)...);
		}
		inline Pitaya::Engine::Thread::ThreadToken RegisterThread(const std::string& name, std::thread::id id) noexcept
		{
			log.LogDebug(name + "线程被注册!");
			return thread.RegisterThread(name, id);
		}
		inline bool UnregisterThread(Pitaya::Engine::Thread::ThreadToken threadToken) noexcept
		{
			log.LogDebug(thread.GetThreadName(threadToken.id) + "线程被解除!");
			return thread.UnregisterThread(threadToken);;
		}
		inline const std::string& GetThreadName(const std::thread::id& id)
		{
			return thread.GetThreadName(id);
		}
#pragma endregion

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
		bool BindEngineModelBackendAPI();
		bool CheckEngineModelBackendBindIsSuccess();

	private:
		Pitaya::Engine::Interface::Renderer* renderer = nullptr;
		Pitaya::Engine::Interface::Physics* physics = nullptr;
		Pitaya::Engine::Interface::Window* window = nullptr;

		Pitaya::Engine::Internal::Input input;
		Pitaya::Engine::Internal::Time time;
		Pitaya::Engine::Internal::Event event;
		Pitaya::Engine::Internal::Log log;
		Pitaya::Engine::Internal::Thread thread;
		//coroutine
		//game
		//camera

		Pitaya::Engine::Config::Config config;
	};
}