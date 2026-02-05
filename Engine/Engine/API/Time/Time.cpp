#include<Engine/API/Time/Time.h>

#include<Engine/Engine.h>
#include<Engine/Internal/Time/Time.h>

float Pitaya::Engine::Time::delta() noexcept
{
	return Pitaya::Engine::Engine::Instance().GetTimeModel()->Getdelta();
}
float Pitaya::Engine::Time::Fixdelta() noexcept
{
	return Pitaya::Engine::Engine::Instance().GetTimeModel()->GetFixdelta();
}
float Pitaya::Engine::Time::UnscaledDelta() noexcept
{
	return Pitaya::Engine::Engine::Instance().GetTimeModel()->GetUnscaledDelta();
}
float Pitaya::Engine::Time::TimeScale() noexcept
{
	return Pitaya::Engine::Engine::Instance().GetTimeModel()->GetTimeScale();
}
float Pitaya::Engine::Time::Seconds() noexcept
{
	return Pitaya::Engine::Engine::Instance().GetTimeModel()->Seconds();
}
int64_t Pitaya::Engine::Time::Milliseconds() noexcept
{
	return Pitaya::Engine::Engine::Instance().GetTimeModel()->Milliseconds();
}