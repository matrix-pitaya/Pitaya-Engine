#include"Time.h"

#include<thread>

bool Pitaya::Engine::Internal::Time::Initialize()
{
	return true;
}
void Pitaya::Engine::Internal::Time::Release()
{

}
void Pitaya::Engine::Internal::Time::Tick()
{
	tick = std::chrono::steady_clock::now();
	unscaledDeltaTime = std::chrono::duration_cast<std::chrono::duration<float>>(tick - lastTick).count();
	delta = unscaledDeltaTime * scale;
	lastTick = tick;
}
void Pitaya::Engine::Internal::Time::FrameSync()
{
	float sleepTime = targetFrameTime - std::chrono::duration_cast<std::chrono::duration<float>>(std::chrono::steady_clock::now() - tick).count();
	if (sleepTime > 0.0f)
	{
		std::this_thread::sleep_for(std::chrono::duration<float>(sleepTime));
	}
}
void Pitaya::Engine::Internal::Time::SetFPS(float fps)
{
	targetFrameTime = 1.0f / fps;
}
float Pitaya::Engine::Internal::Time::Seconds()
{
	return std::chrono::duration_cast<std::chrono::duration<float>>(std::chrono::steady_clock::now() - start).count();
}
int64_t Pitaya::Engine::Internal::Time::Milliseconds()
{
	return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start).count();
}