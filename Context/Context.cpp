#include<Context/Context.h>

template<>
Pitaya::Engine::Context& ENGINE_CALL Pitaya::Core::Singleton<Pitaya::Engine::Context>::Instance()
{
	static Pitaya::Engine::Context instance;
	return instance;
}
