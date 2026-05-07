#include<Time/Chronometer.h>

#if defined(PITAYA_PLATFORM_WINDOWS)
#include<windows.h>
#include<mmsystem.h>
#pragma comment(lib, "winmm.lib") 
#endif

bool Pitaya::Time::Chronometer::Initialize()
{
#if defined(PITAYA_PLATFORM_WINDOWS)
	timeBeginPeriod(1);
#endif
	SetFPS(144.0f);
	start = std::chrono::steady_clock::now();
	tick = start;
	lastTick = start;
	return true;
}
void Pitaya::Time::Chronometer::Release()
{
#if defined(PITAYA_PLATFORM_WINDOWS)
	timeEndPeriod(1);
#endif
}
