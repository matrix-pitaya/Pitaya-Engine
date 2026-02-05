#include<Engine/API/Thread/Thread.h>

#include<Engine/Engine.h>
#include<Engine/Internal/Thread/Thread.h>

Pitaya::Engine::Internal::Thread* Pitaya::Engine::Thread::GetThreadModel() noexcept
{
	return Pitaya::Engine::Engine::Instance().GetThreadModel();
}
const std::string& Pitaya::Engine::Thread::GetThreadName(std::thread::id id) noexcept
{
	return Pitaya::Engine::Engine::Instance().GetThreadModel()->GetThreadName(id);
}