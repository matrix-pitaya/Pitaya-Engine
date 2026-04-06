#include<Engine/API/func.h>
#include<Engine/Engine.h>

int ENGINE_CALL Pitaya::Engine::Execute()
{
	return Pitaya::Engine::Engine::Instance().Execute();
}
void ENGINE_CALL Pitaya::Engine::Terminate()
{
	Pitaya::Engine::Engine::Instance().Terminate();
}
