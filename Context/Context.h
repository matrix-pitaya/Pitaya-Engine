#pragma once

#include<Core/Singleton/Singleton.h>
#include<Context/Common/Module-TypeTraits.h>
#include<Engine/API/def.h>
#include<stdexcept>

namespace Pitaya::Engine 
{ 
	class Engine; 
	class Context; 
	template<typename T> 
	class Module; 
	template<typename T>
	struct FuncTable; 
}

namespace Pitaya::Engine
{
	class Context : public Pitaya::Core::Singleton<Context>
	{
		friend class Pitaya::Core::Singleton<Context>;
		friend class Pitaya::Engine::Engine;
	private:
		struct Modules
		{
			friend class Pitaya::Engine::Engine;
			Pitaya::Engine::Module<Pitaya::Task::TaskScheduler>* TaskScheduler = nullptr;
			Pitaya::Engine::Module<Pitaya::Render::Renderer>* Renderer = nullptr;
			Pitaya::Engine::Module<Pitaya::Render::RenderPipeline>* RenderPipeline = nullptr;
			Pitaya::Engine::Module<Pitaya::Log::Logger>* Logger = nullptr;
			Pitaya::Engine::Module<Pitaya::Event::EventDispatcher>* EventDispatcher = nullptr;
			Pitaya::Engine::Module<Pitaya::Thread::ThreadTracker>* ThreadTracker = nullptr;
			Pitaya::Engine::Module<Pitaya::GPU::RHIDevice>* RHIDevice = nullptr;
			Pitaya::Engine::Module<Pitaya::Time::Chronometer>* Chronometer = nullptr;
			Pitaya::Engine::Module<Pitaya::Input::InputMonitor>* InputMonitor = nullptr;
			Pitaya::Engine::Module<Pitaya::Asset::AssetHub>* AssetHub = nullptr;
			Pitaya::Engine::Module<Pitaya::Config::Configurator>* Configurator = nullptr;
			Pitaya::Engine::Module<Pitaya::Window::Window>* Window = nullptr;
			Pitaya::Engine::Module<Pitaya::Physics::PhysicsSimulator>* PhysicsSimulator = nullptr;
			Pitaya::Engine::Module<Pitaya::Game::GameWorld>* GameWorld = nullptr;
			Pitaya::Engine::Module<Pitaya::Script::ScriptRuntime>* ScriptRuntime = nullptr;
		};
		struct FuncTables
		{
			friend class Pitaya::Engine::Engine;
			Pitaya::Engine::FuncTable<Pitaya::Time::Chronometer>* Chronometer = nullptr;
			Pitaya::Engine::FuncTable<Pitaya::Log::Logger>* Logger = nullptr;
			Pitaya::Engine::FuncTable<Pitaya::Thread::ThreadTracker>* ThreadTracker = nullptr;
			Pitaya::Engine::FuncTable<Pitaya::Asset::AssetHub>* AssetHub = nullptr;
			Pitaya::Engine::FuncTable<Pitaya::Config::Configurator>* Configurator = nullptr;
			Pitaya::Engine::FuncTable<Pitaya::Event::EventDispatcher>* EventDispatcher = nullptr;
			Pitaya::Engine::FuncTable<Pitaya::GPU::RHIDevice>* RHIDevice = nullptr;
			Pitaya::Engine::FuncTable<Pitaya::Input::InputMonitor>* InputMonitor = nullptr;
			Pitaya::Engine::FuncTable<Pitaya::Task::TaskScheduler>* TaskScheduler = nullptr;
			Pitaya::Engine::FuncTable<Pitaya::Window::Window>* Window = nullptr;
			Pitaya::Engine::FuncTable<Pitaya::Game::GameWorld>* GameWorld = nullptr;
			Pitaya::Engine::FuncTable<Pitaya::Render::Renderer>* Renderer = nullptr;
		};

	private:
		Context() = default;
		~Context() = default;
	
	public:
		Context(const Context&) = delete;
		Context& operator=(const Context&) = delete;
		Context(Context&&) = delete;
		Context& operator=(Context&&) = delete;

	public:
		template<ModuleTypeTraits T>
		inline Pitaya::Engine::Module<T>& GetModule() const noexcept
		{
			if constexpr (std::is_same_v<T, Pitaya::Task::TaskScheduler>)			 { return *modules.TaskScheduler; }
			else if constexpr (std::is_same_v<T, Pitaya::Render::Renderer>)			 { return *modules.Renderer; }
			else if constexpr (std::is_same_v<T, Pitaya::Log::Logger>)               { return *modules.Logger; }
			else if constexpr (std::is_same_v<T, Pitaya::Event::EventDispatcher>)    { return *modules.EventDispatcher; }
			else if constexpr (std::is_same_v<T, Pitaya::Thread::ThreadTracker>)     { return *modules.ThreadTracker; }
			else if constexpr (std::is_same_v<T, Pitaya::GPU::RHIDevice>)            { return *modules.RHIDevice; }
			else if constexpr (std::is_same_v<T, Pitaya::Time::Chronometer>)         { return *modules.Chronometer; }
			else if constexpr (std::is_same_v<T, Pitaya::Input::InputMonitor>)       { return *modules.InputMonitor; }
			else if constexpr (std::is_same_v<T, Pitaya::Asset::AssetHub>)           { return *modules.AssetHub; }
			else if constexpr (std::is_same_v<T, Pitaya::Config::Configurator>)      { return *modules.Configurator; }
			else if constexpr (std::is_same_v<T, Pitaya::Window::Window>)            { return *modules.Window; }
			else if constexpr (std::is_same_v<T, Pitaya::Physics::PhysicsSimulator>) { return *modules.PhysicsSimulator; }
			else if constexpr (std::is_same_v<T, Pitaya::Game::GameWorld>)			 { return *modules.GameWorld; }
			else if constexpr (std::is_same_v<T, Pitaya::Script::ScriptRuntime>)	 { return *modules.ScriptRuntime; }
		}
		template<ModuleTypeTraits T>
		inline Pitaya::Engine::FuncTable<T>& GetFuncTable() const noexcept
		{
			if constexpr (std::is_same_v<T, Pitaya::Time::Chronometer>)				{ return *funcTables.Chronometer; }
			else if constexpr (std::is_same_v<T, Pitaya::Log::Logger>)				{ return *funcTables.Logger; }
			else if constexpr (std::is_same_v<T, Pitaya::Thread::ThreadTracker>)	{ return *funcTables.ThreadTracker; }
			else if constexpr (std::is_same_v<T, Pitaya::Asset::AssetHub>)			{ return *funcTables.AssetHub; }
			else if constexpr (std::is_same_v<T, Pitaya::Config::Configurator>)		{ return *funcTables.Configurator; }
			else if constexpr (std::is_same_v<T, Pitaya::Event::EventDispatcher>)	{ return *funcTables.EventDispatcher; }
			else if constexpr (std::is_same_v<T, Pitaya::GPU::RHIDevice>)			{ return *funcTables.RHIDevice; }
			else if constexpr (std::is_same_v<T, Pitaya::Input::InputMonitor>)		{ return *funcTables.InputMonitor; }
			else if constexpr (std::is_same_v<T, Pitaya::Task::TaskScheduler>)		{ return *funcTables.TaskScheduler; }
			else if constexpr (std::is_same_v<T, Pitaya::Window::Window>)			{ return *funcTables.Window; }
			else if constexpr (std::is_same_v<T, Pitaya::Game::GameWorld>)			{ return *funcTables.GameWorld; }
			else if constexpr (std::is_same_v<T, Pitaya::Render::Renderer>)			{ return *funcTables.Renderer; }
		}

	private:
		bool Verify() const;
		void Nullify() noexcept;

	private:
		Modules modules;
		FuncTables funcTables;
	};
}

template<>
ENGINE_API Pitaya::Engine::Context& ENGINE_CALL Pitaya::Core::Singleton<Pitaya::Engine::Context>::Instance();
