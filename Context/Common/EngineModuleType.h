#pragma once

#include<type_traits>

namespace Pitaya::Time { class Chronometer; }
namespace Pitaya::Log { class Logger; }
namespace Pitaya::Thread { class ThreadTracker; }
namespace Pitaya::Event { class EventDispatcher; }
namespace Pitaya::Input { class InputMonitor; }
namespace Pitaya::Task { class TaskScheduler; }
namespace Pitaya::GPU { class RHIDevice; }
namespace Pitaya::Asset { class AssetHub; }
namespace Pitaya::Render { class Renderer; class RenderPipeline; }
namespace Pitaya::Config { class Configurator; }
namespace Pitaya::Window { class Window; }
namespace Pitaya::Physics { class PhysicsSimulator; }
namespace Pitaya::Game { class GameWorld; }
namespace Pitaya::Script { class ScriptRuntime; }

template <typename T>
concept EngineModuleType =
std::is_same_v<T, Pitaya::Time::Chronometer> ||
std::is_same_v<T, Pitaya::Log::Logger> ||
std::is_same_v<T, Pitaya::Thread::ThreadTracker> ||
std::is_same_v<T, Pitaya::Event::EventDispatcher> ||
std::is_same_v<T, Pitaya::Input::InputMonitor> ||
std::is_same_v<T, Pitaya::Task::TaskScheduler> ||
std::is_same_v<T, Pitaya::GPU::RHIDevice> ||
std::is_same_v<T, Pitaya::Asset::AssetHub> ||
std::is_same_v<T, Pitaya::Render::Renderer> ||
std::is_same_v<T, Pitaya::Render::RenderPipeline> || 
std::is_same_v<T, Pitaya::Config::Configurator> ||
std::is_same_v<T, Pitaya::Window::Window> ||
std::is_same_v<T, Pitaya::Physics::PhysicsSimulator> ||
std::is_same_v<T, Pitaya::Game::GameWorld> ||
std::is_same_v<T, Pitaya::Script::ScriptRuntime>;