#include<Window/Backend/GLFW/GLFWWindow.h>
#include<Task/Common/FuncTable.h>
#include<Event/Common/FuncTable.h>
#include<Core/Utils/File.h>
#include<Application/resource.h>	//获取exe图标Id宏

#define GLFW_EXPOSE_NATIVE_WIN32
#include<GLFW/glfw3native.h>  

#include<dwmapi.h>

#pragma comment(lib, "dwmapi.lib") 

bool Pitaya::Window::GLFWWindow::Initialize(int width, int height, const char* title)
{
	//GLFW库
	if (glfwInit() != GLFW_TRUE)
	{
		return false;
	}

	//设置GLFW版本号4.6
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
	//glfwWindowHint(GLFW_SAMPLES, 4);

	//创建窗口
	window = glfwCreateWindow(width, height, title, NULL, NULL);
	//检测窗口是否创建成功
	if (!window)
	{
		glfwTerminate();
		return false;
	}
	glfwMakeContextCurrent(nullptr);
	glfwSetWindowUserPointer(window, this);
	glfwSetFramebufferSizeCallback(window, FramebufferResetSizeCallback);
	glfwSetCursorPosCallback(window, MouseCursorMoveCallback);
	glfwSetScrollCallback(window, MouseScrollCallback);
	glfwSetKeyCallback(window, KeyCallback);
	glfwSetMouseButtonCallback(window, MouseButtonCallback);
	glfwSetWindowCloseCallback(window, WindowCloseCallback);
	glfwSetDropCallback(window, DropFileCallback);

	SetWindowStyle();
	glfwShowWindow(window);
	RegisterKeyMap();

	return true;
}
void Pitaya::Window::GLFWWindow::Release()
{
	glfwDestroyWindow(window);	//销毁窗口
	glfwTerminate();			//卸载库
	window = nullptr;
}
bool Pitaya::Window::GLFWWindow::IsClose() const
{
	return static_cast<bool>(glfwWindowShouldClose(window));
}
void Pitaya::Window::GLFWWindow::PollEvents(Pitaya::Core::PassKey<Pitaya::Engine::Engine>) const
{
	glfwPollEvents();
}
void Pitaya::Window::GLFWWindow::CloseWindow(Pitaya::Core::PassKey<Pitaya::Engine::Engine>) const
{
	glfwSetWindowShouldClose(window, GLFW_TRUE);
}
void* Pitaya::Window::GLFWWindow::GetNativeWindow() const
{
	return window;
}
glm::uvec2 Pitaya::Window::GLFWWindow::GetWindowSize() const
{
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	return { width, height };
}
void Pitaya::Window::GLFWWindow::SetWindowStyle()
{
	// Title
	HWND hwnd = glfwGetWin32Window(window);
	BOOL darkTheme = TRUE;
	DwmSetWindowAttribute(hwnd, 20, &darkTheme, sizeof(darkTheme));
	COLORREF bgColor = RGB(28, 28, 28);
	DwmSetWindowAttribute(hwnd, 35, &bgColor, sizeof(bgColor));
	COLORREF textColor = RGB(235, 235, 235);
	DwmSetWindowAttribute(hwnd, 36, &textColor, sizeof(textColor));
	DWORD backdropType = 1;
	DwmSetWindowAttribute(hwnd, 38, &backdropType, sizeof(backdropType));

	// Icon
	HINSTANCE hInst = GetModuleHandle(NULL);
	HICON hIconBig = (HICON)LoadImage(	// ICON_BIG 对应任务栏图标
		hInst,
		MAKEINTRESOURCE(IDI_ICON1),
		IMAGE_ICON,
		GetSystemMetrics(SM_CXICON),
		GetSystemMetrics(SM_CYICON),
		LR_DEFAULTCOLOR);
	HICON hIconSmall = (HICON)LoadImage(	// ICON_SMALL 对应标题栏图标
		hInst,
		MAKEINTRESOURCE(IDI_ICON1),
		IMAGE_ICON,
		GetSystemMetrics(SM_CXSMICON),
		GetSystemMetrics(SM_CYSMICON),
		LR_DEFAULTCOLOR);
	if (hIconBig) { SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM)hIconBig); }
	if (hIconSmall) { SendMessage(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)hIconSmall); }
}
void Pitaya::Window::GLFWWindow::FramebufferResetSizeCallback(GLFWwindow* glfwWindow, int width, int height)
{
	if ((width > 0) && (height > 0))
	{
		Pitaya::Event::FramebufferResetSizeEventArgs args = Pitaya::Event::FramebufferResetSizeEventArgs(width, height);
		Pitaya::Event::Event event = Pitaya::Event::Event(Pitaya::Event::EventType::WindowFramebufferResetSize, args);
		Pitaya::Event::Emit(event);
	}
}
void Pitaya::Window::GLFWWindow::MouseCursorMoveCallback(GLFWwindow* glfwWindow, double xPosition, double yPosition)
{
	Pitaya::Event::MouseCurrsorMoveEventArgs args = Pitaya::Event::MouseCurrsorMoveEventArgs(xPosition, yPosition);
	Pitaya::Event::Event event = Pitaya::Event::Event(Pitaya::Event::EventType::MouseCurrsorMove, args);
	Pitaya::Event::Emit(event);
}
void Pitaya::Window::GLFWWindow::MouseScrollCallback(GLFWwindow* glfwWindow, double xOffset, double yOffset)
{
	Pitaya::Event::MouseScrollEventArgs args = Pitaya::Event::MouseScrollEventArgs(xOffset, yOffset);
	Pitaya::Event::Event event = Pitaya::Event::Event(Pitaya::Event::EventType::MouseScroll, args);
	Pitaya::Event::Emit(event);
}
void Pitaya::Window::GLFWWindow::KeyCallback(GLFWwindow* glfwWindow, int key, int scancode, int action, int mods)
{
	Pitaya::Window::GLFWWindow* window = static_cast<GLFWWindow*>(glfwGetWindowUserPointer(glfwWindow));
	if (!window) { return; }

	Pitaya::Event::KeyEventArgs args = Pitaya::Event::KeyEventArgs(window->IntToKeyCode(key), scancode, action, mods);
	Pitaya::Event::Event event = Pitaya::Event::Event(Pitaya::Event::EventType::Key, args);
	Pitaya::Event::Emit(event);
}
void Pitaya::Window::GLFWWindow::MouseButtonCallback(GLFWwindow* glfwWindow, int button, int action, int mods)
{
	Pitaya::Window::GLFWWindow* window = static_cast<GLFWWindow*>(glfwGetWindowUserPointer(glfwWindow));
	if (!window) { return; }

	Pitaya::Event::MouseButtonEventArgs args = Pitaya::Event::MouseButtonEventArgs(window->IntToKeyCode(button), action, mods);
	Pitaya::Event::Event event = Pitaya::Event::Event(Pitaya::Event::EventType::MouseButton, args);
	Pitaya::Event::Emit(event);
}
void Pitaya::Window::GLFWWindow::WindowCloseCallback(GLFWwindow* glfwWindow)
{
	Pitaya::Event::CloseEventArgs args = Pitaya::Event::CloseEventArgs();
	Pitaya::Event::Event event = Pitaya::Event::Event(Pitaya ::Event::EventType::WindowClose, args);
	Pitaya::Event::Emit(event);
}
void Pitaya::Window::GLFWWindow::DropFileCallback(GLFWwindow* glfwWindow, int count, const char** paths)
{
	Pitaya::Event::DropFileEventArgs args = Pitaya::Event::DropFileEventArgs(count, paths);
	Pitaya::Event::Event event = Pitaya::Event::Event(Pitaya::Event::EventType::DropFile, args);
	Pitaya::Event::Emit(event);
}
