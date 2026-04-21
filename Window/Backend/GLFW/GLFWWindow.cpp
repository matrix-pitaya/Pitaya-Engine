#include<Window/Backend/GLFW/GLFWWindow.h>
#include<Task/Common/FuncTable.h>
#include<Event/Common/FuncTable.h>
#include<Import/Import.h>
#include<Asset/Common/Texture.h>
#include<Core/Utils/File.h>

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

	SetTitleStyle();
	LoadWindowIcon();
	glfwShowWindow(window);
	
	RegisterKeyMap();
	this->width = width;
	this->height = height;
	
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
void Pitaya::Window::GLFWWindow::ResetSize(int width, int height)
{
	this->width = width;
	this->height = height;
}
void Pitaya::Window::GLFWWindow::SetTitleStyle()
{
	HWND hwnd = glfwGetWin32Window(window);
	BOOL darkTheme = TRUE;
	DwmSetWindowAttribute(hwnd, 20, &darkTheme, sizeof(darkTheme));
	COLORREF bgColor = RGB(28, 28, 28);
	DwmSetWindowAttribute(hwnd, 35, &bgColor, sizeof(bgColor));
	COLORREF textColor = RGB(235, 235, 235);
	DwmSetWindowAttribute(hwnd, 36, &textColor, sizeof(textColor));
	DWORD backdropType = 1;
	DwmSetWindowAttribute(hwnd, 38, &backdropType, sizeof(backdropType));
}
void Pitaya::Window::GLFWWindow::LoadWindowIcon()
{
	Pitaya::Import::Texture2DImportResult result;
	Pitaya::Import::Import(Pitaya::Asset::Texture::Icon, Pitaya::Core::GetExecutableDirectory() / "resource/icon/default.png", false, true, result);
	GLFWimage icon;
	icon.width = result.Width;
	icon.height = result.Height;
	icon.pixels = result.Data.data();
	glfwSetWindowIcon(window, 1, &icon);
}
void Pitaya::Window::GLFWWindow::FramebufferResetSizeCallback(GLFWwindow* glfwWindow, int width, int height)
{
	Pitaya::Window::GLFWWindow* window = static_cast<GLFWWindow*>(glfwGetWindowUserPointer(glfwWindow));
	if (window) { window->ResetSize(width, height); }
	
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
