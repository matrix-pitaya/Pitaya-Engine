#include<Editor/Profiler/Profiler.h>

#include<utility>

bool Pitaya::Editor::Profiler::Initialize()
{
	return true;
}
void Pitaya::Editor::Profiler::Release()
{

}
void Pitaya::Editor::Profiler::BeginFrame()
{
	front.Reset();
}
void Pitaya::Editor::Profiler::EndFrame()
{
	std::swap(front, back);
}