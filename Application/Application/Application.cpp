#include<Application/Application.h>

#include<Engine/API/Engine/Bootstrap.h>
#include<Engine/API/Log/Log.h>
#include<Console/Console.h>

#include<exception>
#include<stdexcept>

int Pitaya::Application::Execute()
{
	try
	{
		return Engine::Execute();
	}
	catch (const std::exception& e)
	{
		Pitaya::Core::Console::Print(e.what());
		Pitaya::Engine::Log::LogError(e.what());
		Pitaya::Core::Console::PopupMessageBox("提示", e.what());
		return -1;
	}
	catch (...)
	{
		Pitaya::Core::Console::Print("未知错误!");
		Pitaya::Engine::Log::LogError("未知错误!");
		Pitaya::Core::Console::PopupMessageBox("提示", "未知错误!");
		return -1;
	}
}