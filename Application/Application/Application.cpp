#include"Application.h"

#include"Engine/EngineAPI.h"

#include"Log/LogAPI.h"
#include"Log/Console.h"

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
		Pitaya::Core::Log::Log(Core::Log::LogLevel::Error, e.what());
		Pitaya::Core::Log::PopupMessageBox("提示", e.what());
		return -1;
	}
	catch (...)
	{
		Pitaya::Core::Console::Print("未知错误!");
		Pitaya::Core::Log::Log(Core::Log::LogLevel::Error, "未知错误!");
		Pitaya::Core::Log::PopupMessageBox("提示", "未知错误!");
		return -1;
	}
}