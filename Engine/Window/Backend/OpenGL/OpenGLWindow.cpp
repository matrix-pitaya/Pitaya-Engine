#include"OpenGLWindow.h"
#include"Event/EventAPI.h"

Pitaya::Engine::Window::OpenGLWindow::OpenGLWindow() = default;
Pitaya::Engine::Window::OpenGLWindow::~OpenGLWindow()
{
	Release();
}

bool Pitaya::Engine::Window::OpenGLWindow::Initialize(int width, int height, const char* title)
{
	if (isInitialized)
	{
		return false;
	}

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
		glfwTerminate();//卸载库
		return false;
	}
	glfwMakeContextCurrent(nullptr);
	glfwSetWindowUserPointer(window, this);
	glfwSetFramebufferSizeCallback(window, FramebufferResetSizeCallback);
	glfwSetCursorPosCallback(window, MouseCursorMoveCallback);
	glfwSetScrollCallback(window, MouseScrollCallback);
	glfwSetKeyCallback(window, KeyDownCallback);
	glfwSetMouseButtonCallback(window, MouseButtonCallback);
	glfwSetWindowCloseCallback(window, WindowCloseCallback);
	glfwSetDropCallback(window, DropFileCallback);

	//TODO: 加载图标
	/*
	int _width, _height, _channel;
	unsigned char* pixels = stbi_load(WindowConfig::HUOLG_ICON_PATH, &_width, &_height, &_channel, 4);
	GLFWimage icon;
	icon.width = _width;
	icon.height = _height;
	icon.pixels = pixels;
	glfwSetWindowIcon(window, 1, &icon);
	stbi_image_free(pixels);
	*/

	this->width = width;
	this->height = height;

	isInitialized = true;
	return true;
}
void Pitaya::Engine::Window::OpenGLWindow::Release()
{
	if (isReleased)
	{
		return;
	}

	isReleased = true;
}
bool Pitaya::Engine::Window::OpenGLWindow::GetKeyDown(Engine::Input::KeyCode keyCode) const
{
	switch (keyCode)
	{
		case Engine::Input::KeyCode::W:
			return glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS;

		case Engine::Input::KeyCode::A:
			return glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS;

		case Engine::Input::KeyCode::S:
			return glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS;

		case Engine::Input::KeyCode::D:
			return glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS;

		case Engine::Input::KeyCode::Mouse0:
			return glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;

		case Engine::Input::KeyCode::Mouse1:
			return glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS;

		case Engine::Input::KeyCode::Mouse2:
			return glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS;

		case Engine::Input::KeyCode::ESC:
			return glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS;

		default:
			return false;
	}
}
bool Pitaya::Engine::Window::OpenGLWindow::IsClose() const
{
	return static_cast<bool>(glfwWindowShouldClose(window));
}
void Pitaya::Engine::Window::OpenGLWindow::ClearFrameBuffer() const
{
	glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
	glClearDepth(1.0f);
	glClearStencil(0x00);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}
void Pitaya::Engine::Window::OpenGLWindow::PollEvents() const
{
	glfwPollEvents();
}
void Pitaya::Engine::Window::OpenGLWindow::SwapBuffer() const
{
	glfwSwapBuffers(window);
}
void Pitaya::Engine::Window::OpenGLWindow::CloseWindow() const
{
	glfwSetWindowShouldClose(window, GLFW_TRUE);
}
void* Pitaya::Engine::Window::OpenGLWindow::GetNativeWindow() const
{
	return window;
}
void Pitaya::Engine::Window::OpenGLWindow::ResetSize(int width, int height)
{
	this->width = width;
	this->height = height;
	glViewport(0, 0, width, height);
}

void Pitaya::Engine::Window::OpenGLWindow::FramebufferResetSizeCallback(GLFWwindow* glfwWindow, int width, int height)
{
	Pitaya::Engine::Window::OpenGLWindow* window = static_cast<OpenGLWindow*>(glfwGetWindowUserPointer(glfwWindow));
	if (!window)
	{
		window->ResetSize(width, height);
	}
	
	if ((width > 0) && (height > 0))
	{
		Core::Event::Args::Window::FramebufferResetSizeEventArgs args = Core::Event::Args::Window::FramebufferResetSizeEventArgs(width, height);
		Core::Event::Event event = Core::Event::Event(Core::Event::EventType::WindowFramebufferResetSize, args);
		Core::Event::Emit(event);
	}
}
void Pitaya::Engine::Window::OpenGLWindow::MouseCursorMoveCallback(GLFWwindow* glfwWindow, double xPosition, double yPosition)
{
	Core::Event::Args::Input::MouseCurrsorMoveEventArgs args = Core::Event::Args::Input::MouseCurrsorMoveEventArgs(xPosition, yPosition);
	Core::Event::Event event = Core::Event::Event(Core::Event::EventType::MouseCurrsorMove, args);
	Core::Event::Emit(event);
}
void Pitaya::Engine::Window::OpenGLWindow::MouseScrollCallback(GLFWwindow* glfwWindow, double xOffset, double yOffset)
{
	Core::Event::Args::Input::MouseScrollEventArgs args = Core::Event::Args::Input::MouseScrollEventArgs(xOffset, yOffset);
	Core::Event::Event event = Core::Event::Event(Core::Event::EventType::MouseScroll, args);
	Core::Event::Emit(event);
}
void Pitaya::Engine::Window::OpenGLWindow::KeyDownCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	Core::Event::Args::Input::KeyDownEventArgs args = Core::Event::Args::Input::KeyDownEventArgs(key, scancode, action, mods);
	Core::Event::Event event = Core::Event::Event(Core::Event::EventType::KeyDown, args);
	Core::Event::Emit(event);
}
void Pitaya::Engine::Window::OpenGLWindow::MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
	Core::Event::Args::Input::MouseButtonEventArgs args = Core::Event::Args::Input::MouseButtonEventArgs(button, action, mods);
	Core::Event::Event event = Core::Event::Event(Core::Event::EventType::MouseButton, args);
	Core::Event::Emit(event);
}
void Pitaya::Engine::Window::OpenGLWindow::WindowCloseCallback(GLFWwindow* window)
{
	Core::Event::Args::Window::CloseEventArgs args = Core::Event::Args::Window::CloseEventArgs();
	Core::Event::Event event = Core::Event::Event(Core::Event::EventType::WindowClose, args);
	Core::Event::Emit(event);
}
void Pitaya::Engine::Window::OpenGLWindow::DropFileCallback(GLFWwindow* window, int count, const char** paths)
{
	Core::Event::Args::Window::DropFileEventArgs args = Core::Event::Args::Window::DropFileEventArgs(count, paths);
	Core::Event::Event event = Core::Event::Event(Core::Event::EventType::DropFile, args);
	Core::Event::Emit(event);
}