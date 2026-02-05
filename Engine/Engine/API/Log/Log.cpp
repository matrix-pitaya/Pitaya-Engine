#include<Engine/API/Log.h>

#include<Engine/Engine.h>
#include<Engine/Internal/Log/Log.h>

void Pitaya::Engine::Log::LogInfo(const std::string& message) noexcept
{
	Pitaya::Engine::Engine::Instance().GetLogModel()->LogInfo(message);
}
void Pitaya::Engine::Log::LogDebug(const std::string& message) noexcept
{
	Pitaya::Engine::Engine::Instance().GetLogModel()->LogDebug(message);
}
void Pitaya::Engine::Log::LogWarning(const std::string& message) noexcept
{
	Pitaya::Engine::Engine::Instance().GetLogModel()->LogWarning(message);
}
void Pitaya::Engine::Log::LogError(const std::string& message) noexcept
{
	Pitaya::Engine::Engine::Instance().GetLogModel()->LogError(message);
}