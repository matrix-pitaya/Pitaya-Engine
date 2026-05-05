#include<Engine/API/func.h>
#include<Engine/Engine.h>

int ENGINE_CALL Pitaya::Engine::Execute(int argc, char** argv)
{
	return Pitaya::Engine::Engine::Instance().Execute(argc, argv);
}
void ENGINE_CALL Pitaya::Engine::Terminate()
{
	Pitaya::Engine::Engine::Instance().Terminate();
}
