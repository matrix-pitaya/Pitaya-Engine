#include"Config.h"

#include<Engine/Engine.h>

size_t Pitaya::Engine::Config::GetMaxFixupdataExecuteTimes() noexcept
{
	return Pitaya::Engine::Engine::Instance().GetConfigModel().MaxFixupdateExecuteTimes;
}