#include<Time/Chronometer.h>
#include<windows.h>
#include<mmsystem.h>

#pragma comment(lib, "winmm.lib") 

bool Pitaya::Time::Chronometer::Initialize()
{
	timeBeginPeriod(1);
	SetFPS(144.0f);
	start = std::chrono::steady_clock::now();
	tick = start;
	lastTick = start;
	return true;
}
void Pitaya::Time::Chronometer::Release()
{
	timeEndPeriod(1);
}
