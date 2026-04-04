#include<Profiler/FrameMonitor.h>

bool Pitaya::Profiler::FrameMonitor::Initialize()
{
	return true;
}
void Pitaya::Profiler::FrameMonitor::Release()
{

}
void Pitaya::Profiler::FrameMonitor::BeginFrame()
{
	front.Reset();
}
void Pitaya::Profiler::FrameMonitor::EndFrame()
{
	std::swap(front, back);
}