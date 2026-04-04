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



//TODO 模拟dll加载，将Editor设置为导出dll后移除
#include<Editor/Editor.h>
void ENGINE_CALL Pitaya::Engine::EditorMountHook()
{
	Pitaya::Editor::Editor::Instance().MountEngineHook();
}