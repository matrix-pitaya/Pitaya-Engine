#include<Context/Context.h>

#define MODULE(T) modules.T
#define FUNCTABLE(T) funcTables.T

bool Pitaya::Engine::Context::Verify() const
{
	//Verify Module
	do
	{
		if (!MODULE(Configurator)) { throw std::runtime_error("Context [Module] miss [Configurator]"); }
		if (!MODULE(InputMonitor)) { throw std::runtime_error("Context [Module] miss [InputMonitor]"); }
		if (!MODULE(Chronometer)) { throw std::runtime_error("Context [Module] miss [Chronometer]"); }
		if (!MODULE(EventDispatcher)) { throw std::runtime_error("Context [Module] miss [EventDispatcher]"); }
		if (!MODULE(Logger)) { throw std::runtime_error("Context [Module] miss [Logger]"); }
		if (!MODULE(ThreadTracker)) { throw std::runtime_error("Context [Module] miss [ThreadTracker]"); }
		if (!MODULE(AssetHub)) { throw std::runtime_error("Context [Module] miss [AssetHub]"); }
		if (!MODULE(TaskScheduler)) { throw std::runtime_error("Context [Module] miss [TaskScheduler]"); }
		if (!MODULE(Renderer)) { throw std::runtime_error("Context [Module] miss [Renderer]"); }
		if (!MODULE(PhysicsSimulator)) { throw std::runtime_error("Context [Module] miss [PhysicsSimulator]"); }
		if (!MODULE(Window)) { throw std::runtime_error("Context [Module] miss [Window]"); }
		if (!MODULE(RHIDevice)) { throw std::runtime_error("Context [Module] miss [RHIDevice]"); }
		if (!MODULE(GameWorld)) { throw std::runtime_error("Context [Module] miss [GameWorld]"); }
		if (!MODULE(ScriptRuntime)) { throw std::runtime_error("Context [Module] miss [ScriptRuntime]"); }
	} while (false);

	//Verify FuncTable
	do
	{
		if (!FUNCTABLE(AssetHub)) { throw std::runtime_error("Context [FuncTable] miss [AssetHub]"); }
		if (!FUNCTABLE(Configurator)) { throw std::runtime_error("Context [FuncTable] miss [Configurator]"); }
		if (!FUNCTABLE(EventDispatcher)) { throw std::runtime_error("Context [FuncTable] miss [EventDispatcher]"); }
		if (!FUNCTABLE(RHIDevice)) { throw std::runtime_error("Context [FuncTable] miss [RHIDevice]"); }
		if (!FUNCTABLE(InputMonitor)) { throw std::runtime_error("Context [FuncTable] miss [InputMonitor]"); }
		if (!FUNCTABLE(Logger)) { throw std::runtime_error("Context [FuncTable] miss [Logger]"); }
		if (!FUNCTABLE(TaskScheduler)) { throw std::runtime_error("Context [FuncTable] miss [TaskScheduler]"); }
		if (!FUNCTABLE(ThreadTracker)) { throw std::runtime_error("Context [FuncTable] miss [ThreadTracker]"); }
		if (!FUNCTABLE(Chronometer)) { throw std::runtime_error("Context [FuncTable] miss [Chronometer]"); }
		if (!FUNCTABLE(Window)) { throw std::runtime_error("Context [FuncTable] miss [Window]"); }
		if (!FUNCTABLE(GameWorld)) { throw std::runtime_error("Context [FuncTable] miss [GameWorld]"); }
		if (!FUNCTABLE(Renderer)) { throw std::runtime_error("Context [FuncTable] miss [Renderer]"); }
	} while (false);
	return true;
}
void Pitaya::Engine::Context::Nullify() noexcept
{
	//Discard Module
	do
	{
		MODULE(Renderer) = nullptr;
		MODULE(RHIDevice) = nullptr;
		MODULE(PhysicsSimulator) = nullptr;
		MODULE(Window) = nullptr;
		MODULE(InputMonitor) = nullptr;
		MODULE(Chronometer) = nullptr;
		MODULE(EventDispatcher) = nullptr;
		MODULE(Logger) = nullptr;
		MODULE(ThreadTracker) = nullptr;
		MODULE(AssetHub) = nullptr;
		MODULE(TaskScheduler) = nullptr;
		MODULE(Configurator) = nullptr;
		MODULE(GameWorld) = nullptr;
		MODULE(ScriptRuntime) = nullptr;
	} while (false);

	//Discard FuncTable
	do
	{
		FUNCTABLE(Chronometer) = nullptr;
		FUNCTABLE(Logger) = nullptr;
		FUNCTABLE(ThreadTracker) = nullptr;
		FUNCTABLE(AssetHub) = nullptr;
		FUNCTABLE(Configurator) = nullptr;
		FUNCTABLE(EventDispatcher) = nullptr;
		FUNCTABLE(RHIDevice) = nullptr;
		FUNCTABLE(InputMonitor) = nullptr;
		FUNCTABLE(TaskScheduler) = nullptr;
		FUNCTABLE(Window) = nullptr;
		FUNCTABLE(GameWorld) = nullptr;
		FUNCTABLE(Renderer) = nullptr;
	} while (false);
}

#undef MODULE
#undef FUNCTABLE

template<>
Pitaya::Engine::Context& ENGINE_CALL Pitaya::Core::Singleton<Pitaya::Engine::Context>::Instance()
{
	static Pitaya::Engine::Context instance;
	return instance;
}
