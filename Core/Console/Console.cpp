#include<Core/Console/Console.h>

#include<windows.h>

void Pitaya::Core::PopupMessageBox(const char* title, const char* message)
{
	MessageBoxA(NULL, message, title, MB_OK);
}
