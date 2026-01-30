#pragma once

#include"../Enum/Enum.h"

#include<string>

namespace Pitaya::Core::Log
{
	void Log(LogLevel level,const std::string& message);
	void PopupMessageBox(const std::string& title, const std::string& message);
	void PopupMessageBox(const char* title, const char* message);
}