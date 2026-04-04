#pragma once

#include<Context/Context.h>
#include<Context/Common/Module.h>
#include<Context/Common/FuncTable.h>
#include<Asset/Common/FuncTable.h>
#include<Config/Common/FunctionTable.h>
#include<GPU/Common/FuncTable.h>
#include<Event/Common/FuncTable.h>
#include<Task/Common/FuncTable.h>
#include<Input/Common/FuncTable.h>
#include<Thread/Common/FuncTable.h>
#include<Log/Common/FuncTable.h>
#include<Time/Common/FuncTable.h>
#include<Window/Common/FuncTable.h>

namespace Pitaya::Engine
{
	class Engine : public Pitaya::Core::Singleton<Engine>
	{
		friend class Pitaya::Core::Singleton<Engine>;
	private:
		struct Modules
		{
			Pitaya::Engine::Module<Pitaya::Task::TaskScheduler> TaskScheduler;
			Pitaya::Engine::Module<Pitaya::Render::Renderer> Renderer;
			Pitaya::Engine::Module<Pitaya::Render::RenderPipeline> RenderPipeline;
			Pitaya::Engine::Module<Pitaya::Log::Logger> Logger;
			Pitaya::Engine::Module<Pitaya::Event::EventDispatcher> EventDispatcher;
			Pitaya::Engine::Module<Pitaya::Thread::ThreadTracker> ThreadTracker;
			Pitaya::Engine::Module<Pitaya::GPU::RHIDevice> RHIDevice;
			Pitaya::Engine::Module<Pitaya::Time::Chronometer> Chronometer;
			Pitaya::Engine::Module<Pitaya::Input::InputMonitor> InputMonitor;
			Pitaya::Engine::Module<Pitaya::Asset::AssetHub> AssetHub;
			Pitaya::Engine::Module<Pitaya::Project::Workspace> Workspace;
			Pitaya::Engine::Module<Pitaya::Window::Window> Window;
			Pitaya::Engine::Module<Pitaya::Physics::PhysicsSimulator> PhysicsSimulator;
			Pitaya::Engine::Module<Pitaya::Config::Configurator> Configurator;

			bool Create();
			bool Check();
			bool Initialize(int argc, char** argv);
			bool IsRunning() const;
			void BeginFrame();
			void FixedUpdate();
			void Update();
			void LateUpdate();
			void Render();
			void EndFrame();
			void FrameSync();
			void Release();
			void Destroy();
		};
		struct FuncTables
		{
			Pitaya::Engine::FuncTable<Pitaya::Time::Chronometer> Chronometer;
			Pitaya::Engine::FuncTable<Pitaya::Log::Logger> Logger;
			Pitaya::Engine::FuncTable<Pitaya::Thread::ThreadTracker> ThreadTracker;
			Pitaya::Engine::FuncTable<Pitaya::Asset::AssetHub> AssetHub;
			Pitaya::Engine::FuncTable<Pitaya::Config::Configurator> Configurator;
			Pitaya::Engine::FuncTable<Pitaya::Event::EventDispatcher> EventDispatcher;
			Pitaya::Engine::FuncTable<Pitaya::GPU::RHIDevice> RHIDevice;
			Pitaya::Engine::FuncTable<Pitaya::Input::InputMonitor> InputMonitor;
			Pitaya::Engine::FuncTable<Pitaya::Task::TaskScheduler> TaskScheduler;
			Pitaya::Engine::FuncTable<Pitaya::Window::Window> Window;

			bool Check() const;
			void UnRegister();
		};

	private:
		Engine() = default;
		~Engine() = default;

	public:
		Engine(const Engine&) = delete;
		Engine& operator=(const Engine&) = delete;
		Engine(Engine&&) = delete;
		Engine& operator=(Engine&&) = delete;

	public:
		int Execute(int argc, char** argv);
		void Terminate();

	private:
		bool Initialize(int argc, char** argv);
		bool IsRunning() const;
		void BeginFrame();
		void FixedUpdate();
		void Update();
		void LateUpdate();
		void Render();
		void EndFrame();
		void FrameSync();
		void Release();

	private:
		bool FillContext();
		bool FillFuncTables();

	private:
		Modules modules;
		FuncTables funcTables;
	};
}
