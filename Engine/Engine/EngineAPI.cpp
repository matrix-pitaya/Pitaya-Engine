#include"EngineAPI.h"
#include"Engine.h"

int Pitaya::Engine::Execute()
{
	return Pitaya::Engine::Engine::Instance().Execute();
}

size_t Pitaya::Engine::Config::GetMaxFixupdataExecuteTimes() noexcept
{
	return Pitaya::Engine::Engine::Instance().GetMaxFixupdataExecuteTimes();
}

const std::string& Pitaya::Engine::Thread::GetThreadName(const std::thread::id& id) noexcept
{
	return Pitaya::Engine::Engine::Instance().GetThreadName(id);
}

void Pitaya::Engine::Log::LogInfo(const std::string& message) noexcept
{
	Pitaya::Engine::Engine::Instance().LogInfo(message);
}
void Pitaya::Engine::Log::LogDebug(const std::string& message) noexcept
{
	Pitaya::Engine::Engine::Instance().LogDebug(message);
}
void Pitaya::Engine::Log::LogWarning(const std::string& message) noexcept
{
	Pitaya::Engine::Engine::Instance().LogWarning(message);
}
void Pitaya::Engine::Log::LogError(const std::string& message) noexcept
{
	Pitaya::Engine::Engine::Instance().LogError(message);
}

Pitaya::Engine::Event::EventToken Pitaya::Engine::Event::Subscribe(::Pitaya::Engine::Event::EventType type, std::function<void(const ::Pitaya::Engine::Event::Event&)> function) noexcept
{
	return Pitaya::Engine::Engine::Instance().SubscribeEvent(type, std::move(function));
}
bool Pitaya::Engine::Event::UnSubscribe(const ::Pitaya::Engine::Event::EventToken& eventToken) noexcept
{
	return Pitaya::Engine::Engine::Instance().UnSubscribeEvent(eventToken);
}
void Pitaya::Engine::Event::Emit(const ::Pitaya::Engine::Event::Event& event) noexcept
{
	Pitaya::Engine::Engine::Instance().EmitEvent(event);
}

const float& Pitaya::Engine::Time::delta = Pitaya::Engine::Engine::Instance().GetDeltaTime();
const float& Pitaya::Engine::Time::fixdelta = Pitaya::Engine::Engine::Instance().GetFixdeltaTime();
const float& Pitaya::Engine::Time::unscaledDeltaTime = Pitaya::Engine::Engine::Instance().GetUnscaledDeltaTime();
float& Pitaya::Engine::Time::scale = Pitaya::Engine::Engine::Instance().GetTimeScale();
float Pitaya::Engine::Time::Seconds() noexcept
{
	return Pitaya::Engine::Engine::Instance().GetSecondsTime();
}
int64_t Pitaya::Engine::Time::Milliseconds() noexcept
{
	return Pitaya::Engine::Engine::Instance().GetMillisecondsTime();
}