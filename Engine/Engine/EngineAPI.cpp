#include"EngineAPI.h"
#include"Engine.h"

int Pitaya::Engine::Execute()
{
	return Pitaya::Engine::Engine::Instance.Execute();
}

const float& Pitaya::Engine::Time::delta = Pitaya::Engine::Engine::Engine::Instance.GetDeltaTime();
const float& Pitaya::Engine::Time::fixdelta = Pitaya::Engine::Engine::Engine::Instance.GetFixdeltaTime();
const float& Pitaya::Engine::Time::unscaledDeltaTime = Pitaya::Engine::Engine::Engine::Instance.GetUnscaledDeltaTime();
float& Pitaya::Engine::Time::scale = Pitaya::Engine::Engine::Engine::Instance.GetTimeScale();
float Pitaya::Engine::Time::Seconds()
{
	return Pitaya::Engine::Engine::Instance.GetSecondsTime();
}
int64_t Pitaya::Engine::Time::Milliseconds()
{
	return Pitaya::Engine::Engine::Instance.GetMillisecondsTime();
}