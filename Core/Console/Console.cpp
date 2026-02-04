#include"Console.h"

#include<windows.h>

void Pitaya::Core::Console::PopupMessageBox(const char* title, const char* message)
{
	MessageBoxA(NULL, message, title, MB_OK);
}