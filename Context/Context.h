#pragma once

#include<Core/Singleton/Singleton.h>
#include<stdexcept>
#include<type_traits>

namespace Pitaya::Engine { class Engine; class Context; template<typename T> class Module; template<typename T> struct FuncTable; }
namespace Pitaya::Time { class Chronometer; }
namespace Pitaya::Log { class Logger; }
namespace Pitaya::Thread { class ThreadTracker; }
namespace Pitaya::Event { class EventDispatcher; }
namespace Pitaya::Input { class InputMonitor; }
namespace Pitaya::Task { class TaskScheduler; }
namespace Pitaya::GPU { class RHIDevice; }
namespace Pitaya::Asset { class AssetHub; }
namespace Pitaya::Render { class Renderer; class RenderPipeline; }
namespace Pitaya::Project { class Workspace; }
namespace Pitaya::Config { class Configurator; }
namespace Pitaya::Window { class Window; }
namespace Pitaya::Physics { class PhysicsSimulator; }
namespace Pitaya::Game { class GameWorld; }
namespace Pitaya::Script { class ScriptRuntime; }

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
			Pitaya::Engine::Module<Pitaya::Project::Workspace>* Workspace = nullptr;
			Pitaya::Engine::Module<Pitaya::Config::Configurator>* Configurator = nullptr;
			Pitaya::Engine::Module<Pitaya::Window::Window>* Window = nullptr;
			Pitaya::Engine::Module<Pitaya::Physics::PhysicsSimulator>* PhysicsSimulator = nullptr;
			Pitaya::Engine::Module<Pitaya::Game::GameWorld>* GameWorld = nullptr;
			Pitaya::Engine::Module<Pitaya::Script::ScriptRuntime>* ScriptRuntime = nullptr;

			inline bool Check() const
			{
				if (!Configurator) { throw std::runtime_error("Context [Module] miss [Configurator]"); }
				if (!Workspace) { throw std::runtime_error("Context [Module] miss [Workspace]"); }
				if (!InputMonitor) { throw std::runtime_error("Context [Module] miss [InputMonitor]"); }
				if (!Chronometer) { throw std::runtime_error("Context [Module] miss [Chronometer]"); }
				if (!EventDispatcher) { throw std::runtime_error("Context [Module] miss [EventDispatcher]"); }
				if (!Logger) { throw std::runtime_error("Context [Module] miss [Logger]"); }
				if (!ThreadTracker) { throw std::runtime_error("Context [Module] miss [ThreadTracker]"); }
				if (!AssetHub) { throw std::runtime_error("Context [Module] miss [AssetHub]"); }
				if (!TaskScheduler) { throw std::runtime_error("Context [Module] miss [TaskScheduler]"); }
				if (!Renderer) { throw std::runtime_error("Context [Module] miss [Renderer]"); }
				if (!PhysicsSimulator) { throw std::runtime_error("Context [Module] miss [PhysicsSimulator]"); }
				if (!Window) { throw std::runtime_error("Context [Module] miss [Window]"); }
				if (!RHIDevice) { throw std::runtime_error("Context [Module] miss [RHIDevice]"); }
				//if (!GameWorld) { throw std::runtime_error("Context [Module] miss [GameWorld]"); }
				//if (!ScriptRuntime) { throw std::runtime_error("Context [Module] miss [ScriptRuntime]"); }
				return true;
			}
			inline void UnRegister() noexcept
			{
				Renderer = nullptr;
				RHIDevice = nullptr;
				PhysicsSimulator = nullptr;
				Window = nullptr;
				InputMonitor = nullptr;
				Chronometer = nullptr;
				EventDispatcher = nullptr;
				Logger = nullptr;
				ThreadTracker = nullptr;
				AssetHub = nullptr;
				TaskScheduler = nullptr;
				Workspace = nullptr;
				Configurator = nullptr;
				GameWorld = nullptr;
				ScriptRuntime = nullptr;
			}
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

			inline bool Check() const
			{
				if (!AssetHub) { throw std::runtime_error("Context [FuncTable] miss [AssetHub]"); }
				if (!Configurator) { throw std::runtime_error("Context [FuncTable] miss [Configurator]"); }
				if (!EventDispatcher) { throw std::runtime_error("Context [FuncTable] miss [EventDispatcher]"); }
				if (!RHIDevice) { throw std::runtime_error("Context [FuncTable] miss [RHIDevice]"); }
				if (!InputMonitor) { throw std::runtime_error("Context [FuncTable] miss [InputMonitor]"); }
				if (!Logger) { throw std::runtime_error("Context [FuncTable] miss [Logger]"); }
				if (!TaskScheduler) { throw std::runtime_error("Context [FuncTable] miss [TaskScheduler]"); }
				if (!ThreadTracker) { throw std::runtime_error("Context [FuncTable] miss [ThreadTracker]"); }
				if (!Chronometer) { throw std::runtime_error("Context [FuncTable] miss [Chronometer]"); }
				if (!Window) { throw std::runtime_error("Context [FuncTable] miss [Window]"); }
				return true;
			}
			inline void UnRegister() noexcept
			{
				Chronometer = nullptr;
				Logger = nullptr;
				ThreadTracker = nullptr;
				AssetHub = nullptr;
				Configurator = nullptr;
				EventDispatcher = nullptr;
				RHIDevice = nullptr;
				InputMonitor = nullptr;
				TaskScheduler = nullptr;
				Window = nullptr;
			}
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
		template<typename T>
		inline Pitaya::Engine::Module<T>& GetModule() const noexcept
		{
			static_assert(
				std::is_same_v<T, Pitaya::Task::TaskScheduler> ||
				std::is_same_v<T, Pitaya::Render::Renderer> ||
				std::is_same_v<T, Pitaya::Log::Logger> ||
				std::is_same_v<T, Pitaya::Event::EventDispatcher> ||
				std::is_same_v<T, Pitaya::Thread::ThreadTracker> ||
				std::is_same_v<T, Pitaya::GPU::RHIDevice> ||
				std::is_same_v<T, Pitaya::Time::Chronometer> ||
				std::is_same_v<T, Pitaya::Input::InputMonitor> ||
				std::is_same_v<T, Pitaya::Asset::AssetHub> ||
				std::is_same_v<T, Pitaya::Project::Workspace> ||
				std::is_same_v<T, Pitaya::Config::Configurator> ||
				std::is_same_v<T, Pitaya::Window::Window> ||
				std::is_same_v<T, Pitaya::Physics::PhysicsSimulator> ||
				std::is_same_v<T, Pitaya::Game::GameWorld> ||
				std::is_same_v<T, Pitaya::Script::ScriptRuntime>
				, "Context::GetModule<T>(): T must be a valid Engine Module Type");

			if constexpr (std::is_same_v<T, Pitaya::Task::TaskScheduler>)			 { return *modules.TaskScheduler; }
			else if constexpr (std::is_same_v<T, Pitaya::Render::Renderer>)			 { return *modules.Renderer; }
			else if constexpr (std::is_same_v<T, Pitaya::Log::Logger>)               { return *modules.Logger; }
			else if constexpr (std::is_same_v<T, Pitaya::Event::EventDispatcher>)    { return *modules.EventDispatcher; }
			else if constexpr (std::is_same_v<T, Pitaya::Thread::ThreadTracker>)     { return *modules.ThreadTracker; }
			else if constexpr (std::is_same_v<T, Pitaya::GPU::RHIDevice>)            { return *modules.RHIDevice; }
			else if constexpr (std::is_same_v<T, Pitaya::Time::Chronometer>)         { return *modules.Chronometer; }
			else if constexpr (std::is_same_v<T, Pitaya::Input::InputMonitor>)       { return *modules.InputMonitor; }
			else if constexpr (std::is_same_v<T, Pitaya::Asset::AssetHub>)           { return *modules.AssetHub; }
			else if constexpr (std::is_same_v<T, Pitaya::Project::Workspace>)        { return *modules.Workspace; }
			else if constexpr (std::is_same_v<T, Pitaya::Config::Configurator>)      { return *modules.Configurator; }
			else if constexpr (std::is_same_v<T, Pitaya::Window::Window>)            { return *modules.Window; }
			else if constexpr (std::is_same_v<T, Pitaya::Physics::PhysicsSimulator>) { return *modules.PhysicsSimulator; }
			else if constexpr (std::is_same_v<T, Pitaya::Game::GameWorld>)			 { return *modules.GameWorld; }
			else if constexpr (std::is_same_v<T, Pitaya::Script::ScriptRuntime>)	 { return *modules.ScriptRuntime; }
		}
		template<typename T>
		inline Pitaya::Engine::FuncTable<T>& GetFuncTable() const noexcept
		{
			static_assert(
				std::is_same_v<T, Pitaya::Time::Chronometer> ||
				std::is_same_v<T, Pitaya::Log::Logger> ||
				std::is_same_v<T, Pitaya::Thread::ThreadTracker> ||
				std::is_same_v<T, Pitaya::Asset::AssetHub> ||
				std::is_same_v<T, Pitaya::Config::Configurator> ||
				std::is_same_v<T, Pitaya::Event::EventDispatcher> ||
				std::is_same_v<T, Pitaya::GPU::RHIDevice> ||
				std::is_same_v<T, Pitaya::Input::InputMonitor> ||
				std::is_same_v<T, Pitaya::Task::TaskScheduler> ||
				std::is_same_v<T, Pitaya::Window::Window>
				, "Context::GetFuncTable<T>(): T must be a valid Engine FuncTable Type");

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
		}

	private:
		inline bool Check() const
		{
			return modules.Check() && funcTables.Check();
		}
		inline void UnRegister() noexcept
		{
			modules.UnRegister();
			funcTables.UnRegister();
		}

	private:
		Modules modules;
		FuncTables funcTables;
	};
}
