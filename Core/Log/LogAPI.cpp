#include"Logapi.h"
#include"Log.h"

#include<windows.h>
#include<tchar.h>

void Pitaya::Core::Log::Log(LogLevel level, const std::string& message)
{
	Pitaya::Core::Log::LogManager::Instance.Log(level, message);
}
void Pitaya::Core::Log::PopupMessageBox(const std::string& title, const std::string& message)
{
	MessageBoxA(NULL, message.c_str(), title.c_str(), MB_OK);
}
void Pitaya::Core::Log::PopupMessageBox(const char* title, const char* message)
{
	MessageBoxA(NULL, message, title, MB_OK);
}