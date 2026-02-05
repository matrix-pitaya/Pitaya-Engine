#include<Application/Application.h>

//最终发布时使用Application→连接器→系统→子系统→窗口 避免弹出cmd命令行窗口 (若需要调试cmd命令行窗口 请设置为控制台)
//#pragma comment(linker, "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")

int main()
{
	return Pitaya::Application::Execute();
}