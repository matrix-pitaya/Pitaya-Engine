#include<Window/Platform/GLFW/GLFWWindow.h>

#include<Engine/API/Event/Event.h>

bool Pitaya::Engine::Window::GLFWWindow::Initialize(int width, int height, const char* title)
{
	//GLFW库
	if (glfwInit() != GLFW_TRUE)
	{
		return false;
	}

	//设置GLFW版本号3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwSwapInterval(1);

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

	LoadWindowIcon();
	RegisterKeyMap();

	this->width = width;
	this->height = height;
	
	return true;
}
void Pitaya::Engine::Window::GLFWWindow::Release()
{
	glfwDestroyWindow(window);	//销毁窗口
	glfwTerminate();			//卸载库
	window = nullptr;
}
bool Pitaya::Engine::Window::GLFWWindow::IsClose() const
{
	return static_cast<bool>(glfwWindowShouldClose(window));
}
void Pitaya::Engine::Window::GLFWWindow::PollEvents() const
{
	glfwPollEvents();
}
void Pitaya::Engine::Window::GLFWWindow::CloseWindow() const
{
	glfwSetWindowShouldClose(window, GLFW_TRUE);
}
void* Pitaya::Engine::Window::GLFWWindow::GetNativeWindow() const
{
	return window;
}
void Pitaya::Engine::Window::GLFWWindow::ResetSize(int width, int height)
{
	this->width = width;
	this->height = height;
	glViewport(0, 0, width, height);
}
void Pitaya::Engine::Window::GLFWWindow::LoadWindowIcon()
{
	/*  TODO: 加载图标
int _width, _height, _channel;
unsigned char* pixels = stbi_load(WindowConfig::HUOLG_ICON_PATH, &_width, &_height, &_channel, 4);
GLFWimage icon;
icon.width = _width;
icon.height = _height;
icon.pixels = pixels;
glfwSetWindowIcon(window, 1, &icon);
stbi_image_free(pixels);
*/
}
void Pitaya::Engine::Window::GLFWWindow::FramebufferResetSizeCallback(GLFWwindow* glfwWindow, int width, int height)
{
	Pitaya::Engine::Window::GLFWWindow* window = static_cast<GLFWWindow*>(glfwGetWindowUserPointer(glfwWindow));
	if (window)
	{
		window->ResetSize(width, height);
	}
	
	if ((width > 0) && (height > 0))
	{
		Pitaya::Engine::Event::Args::Window::FramebufferResetSizeEventArgs args = Pitaya::Engine::Event::Args::Window::FramebufferResetSizeEventArgs(width, height);
		Pitaya::Engine::Event::Event event = Pitaya::Engine::Event::Event(Pitaya::Engine::Event::EventType::WindowFramebufferResetSize, args);
		Pitaya::Engine::Event::Emit(event);
	}
}
void Pitaya::Engine::Window::GLFWWindow::MouseCursorMoveCallback(GLFWwindow* glfwWindow, double xPosition, double yPosition)
{
	Pitaya::Engine::Event::Args::Input::MouseCurrsorMoveEventArgs args = Pitaya::Engine::Event::Args::Input::MouseCurrsorMoveEventArgs(xPosition, yPosition);
	Pitaya::Engine::Event::Event event = Pitaya::Engine::Event::Event(Pitaya::Engine::Event::EventType::MouseCurrsorMove, args);
	Pitaya::Engine::Event::Emit(event);
}
void Pitaya::Engine::Window::GLFWWindow::MouseScrollCallback(GLFWwindow* glfwWindow, double xOffset, double yOffset)
{
	Pitaya::Engine::Event::Args::Input::MouseScrollEventArgs args = Pitaya::Engine::Event::Args::Input::MouseScrollEventArgs(xOffset, yOffset);
	Pitaya::Engine::Event::Event event = Pitaya::Engine::Event::Event(Pitaya::Engine::Event::EventType::MouseScroll, args);
	Pitaya::Engine::Event::Emit(event);
}
void Pitaya::Engine::Window::GLFWWindow::KeyCallback(GLFWwindow* glfwWindow, int key, int scancode, int action, int mods)
{
	Pitaya::Engine::Window::GLFWWindow* window = static_cast<GLFWWindow*>(glfwGetWindowUserPointer(glfwWindow));
	if (!window)
	{
		return;
	}

	Pitaya::Engine::Event::Args::Input::KeyEventArgs args = Pitaya::Engine::Event::Args::Input::KeyEventArgs(window->IntToKeyCode(key), scancode, action, mods);
	Pitaya::Engine::Event::Event event = Pitaya::Engine::Event::Event(Pitaya::Engine::Event::EventType::Key, args);
	Pitaya::Engine::Event::Emit(event);
}
void Pitaya::Engine::Window::GLFWWindow::MouseButtonCallback(GLFWwindow* glfwWindow, int button, int action, int mods)
{
	Pitaya::Engine::Window::GLFWWindow* window = static_cast<GLFWWindow*>(glfwGetWindowUserPointer(glfwWindow));
	if (!window)
	{
		return;
	}

	Pitaya::Engine::Event::Args::Input::MouseButtonEventArgs args = Pitaya::Engine::Event::Args::Input::MouseButtonEventArgs(window->IntToKeyCode(button), action, mods);
	Pitaya::Engine::Event::Event event = Pitaya::Engine::Event::Event(Pitaya::Engine::Event::EventType::MouseButton, args);
	Pitaya::Engine::Event::Emit(event);
}
void Pitaya::Engine::Window::GLFWWindow::WindowCloseCallback(GLFWwindow* glfwWindow)
{
	Pitaya::Engine::Event::Args::Window::CloseEventArgs args = Pitaya::Engine::Event::Args::Window::CloseEventArgs();
	Pitaya::Engine::Event::Event event = Pitaya::Engine::Event::Event(Pitaya::Engine::Event::EventType::WindowClose, args);
	Pitaya::Engine::Event::Emit(event);
}
void Pitaya::Engine::Window::GLFWWindow::DropFileCallback(GLFWwindow* glfwWindow, int count, const char** paths)
{
	Pitaya::Engine::Event::Args::Window::DropFileEventArgs args = Pitaya::Engine::Event::Args::Window::DropFileEventArgs(count, paths);
	Pitaya::Engine::Event::Event event = Pitaya::Engine::Event::Event(Pitaya::Engine::Event::EventType::DropFile, args);
	Pitaya::Engine::Event::Emit(event);
}