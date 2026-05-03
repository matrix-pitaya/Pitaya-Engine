#include<Window/Frontend/Window.h>
#include<Task/Common/FuncTable.h>
#include<Event/Common/FuncTable.h>
#include<Core/Utils/File.h>
#include<Application/resource.h>	//获取exe图标Id宏
#include<algorithm>

#if defined(PITAYA_USE_GLFW)
#include<GLFW/glfw3.h>
#include<GLFW/glfw3native.h>  

#include<dwmapi.h>

#pragma comment(lib, "dwmapi.lib") 

namespace
{
	struct GLFWWindowContext
	{
		GLFWwindow* Window = nullptr;
	};

	void FramebufferResetSizeCallback(GLFWwindow* glfwWindow, int width, int height)
	{
		if ((width > 0) && (height > 0))
		{
			Pitaya::Event::FramebufferResetSizeEventArgs args = Pitaya::Event::FramebufferResetSizeEventArgs(width, height);
			Pitaya::Event::Event event = Pitaya::Event::Event(Pitaya::Event::EventType::WindowFramebufferResetSize, args);
			Pitaya::Event::Emit(event);
		}
	}
	void MouseCursorMoveCallback(GLFWwindow* glfwWindow, double xPosition, double yPosition)
	{
		Pitaya::Event::MouseCurrsorMoveEventArgs args = Pitaya::Event::MouseCurrsorMoveEventArgs(xPosition, yPosition);
		Pitaya::Event::Event event = Pitaya::Event::Event(Pitaya::Event::EventType::MouseCurrsorMove, args);
		Pitaya::Event::Emit(event);
	}
	void MouseScrollCallback(GLFWwindow* glfwWindow, double xOffset, double yOffset)
	{
		Pitaya::Event::MouseScrollEventArgs args = Pitaya::Event::MouseScrollEventArgs(xOffset, yOffset);
		Pitaya::Event::Event event = Pitaya::Event::Event(Pitaya::Event::EventType::MouseScroll, args);
		Pitaya::Event::Emit(event);
	}
	void KeyCallback(GLFWwindow* glfwWindow, int key, int scancode, int action, int mods)
	{
		Pitaya::Window::Window* window = static_cast<Pitaya::Window::Window*>(glfwGetWindowUserPointer(glfwWindow));
		if (!window) { return; }

		Pitaya::Event::KeyEventArgs args = Pitaya::Event::KeyEventArgs(window->ToKeyCode(key), scancode, action, mods);
		Pitaya::Event::Event event = Pitaya::Event::Event(Pitaya::Event::EventType::Key, args);
		Pitaya::Event::Emit(event);
	}
	void MouseButtonCallback(GLFWwindow* glfwWindow, int button, int action, int mods)
	{
		Pitaya::Window::Window* window = static_cast<Pitaya::Window::Window*>(glfwGetWindowUserPointer(glfwWindow));
		if (!window) { return; }

		Pitaya::Event::MouseButtonEventArgs args = Pitaya::Event::MouseButtonEventArgs(window->ToKeyCode(button), action, mods);
		Pitaya::Event::Event event = Pitaya::Event::Event(Pitaya::Event::EventType::MouseButton, args);
		Pitaya::Event::Emit(event);
	}
	void WindowCloseCallback(GLFWwindow* glfwWindow)
	{
		Pitaya::Event::CloseEventArgs args = Pitaya::Event::CloseEventArgs();
		Pitaya::Event::Event event = Pitaya::Event::Event(Pitaya::Event::EventType::WindowClose, args);
		Pitaya::Event::Emit(event);
	}
	void DropFileCallback(GLFWwindow* glfwWindow, int count, const char** paths)
	{
		Pitaya::Event::DropFileEventArgs args = Pitaya::Event::DropFileEventArgs(count, paths);
		Pitaya::Event::Event event = Pitaya::Event::Event(Pitaya::Event::EventType::DropFile, args);
		Pitaya::Event::Emit(event);
	}

	inline void SetGLFWWindowStyle(GLFWwindow* glfwWindow)
	{
		// Title
		HWND hwnd = glfwGetWin32Window(glfwWindow);
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
	template <size_t N>
	inline void FillGLFWWindowKeyMap(Pitaya::Input::KeyCode(&map)[N]) noexcept
	{
		std::fill(std::begin(map), std::end(map), Pitaya::Input::KeyCode::Unknown);

		//鼠标
		map[GLFW_MOUSE_BUTTON_1] = Pitaya::Input::KeyCode::Mouse0;
		map[GLFW_MOUSE_BUTTON_2] = Pitaya::Input::KeyCode::Mouse1;
		map[GLFW_MOUSE_BUTTON_3] = Pitaya::Input::KeyCode::Mouse2;
		map[GLFW_MOUSE_BUTTON_4] = Pitaya::Input::KeyCode::Mouse3;
		map[GLFW_MOUSE_BUTTON_5] = Pitaya::Input::KeyCode::Mouse4;

		//0-9
		map[GLFW_KEY_0] = Pitaya::Input::KeyCode::Key0;
		map[GLFW_KEY_1] = Pitaya::Input::KeyCode::Key1;
		map[GLFW_KEY_2] = Pitaya::Input::KeyCode::Key2;
		map[GLFW_KEY_3] = Pitaya::Input::KeyCode::Key3;
		map[GLFW_KEY_4] = Pitaya::Input::KeyCode::Key4;
		map[GLFW_KEY_5] = Pitaya::Input::KeyCode::Key5;
		map[GLFW_KEY_6] = Pitaya::Input::KeyCode::Key6;
		map[GLFW_KEY_7] = Pitaya::Input::KeyCode::Key7;
		map[GLFW_KEY_8] = Pitaya::Input::KeyCode::Key8;
		map[GLFW_KEY_9] = Pitaya::Input::KeyCode::Key9;

		//A-Z
		map[GLFW_KEY_A] = Pitaya::Input::KeyCode::A;
		map[GLFW_KEY_B] = Pitaya::Input::KeyCode::B;
		map[GLFW_KEY_C] = Pitaya::Input::KeyCode::C;
		map[GLFW_KEY_D] = Pitaya::Input::KeyCode::D;
		map[GLFW_KEY_E] = Pitaya::Input::KeyCode::E;
		map[GLFW_KEY_F] = Pitaya::Input::KeyCode::F;
		map[GLFW_KEY_G] = Pitaya::Input::KeyCode::G;
		map[GLFW_KEY_H] = Pitaya::Input::KeyCode::H;
		map[GLFW_KEY_I] = Pitaya::Input::KeyCode::I;
		map[GLFW_KEY_J] = Pitaya::Input::KeyCode::J;
		map[GLFW_KEY_K] = Pitaya::Input::KeyCode::K;
		map[GLFW_KEY_L] = Pitaya::Input::KeyCode::L;
		map[GLFW_KEY_M] = Pitaya::Input::KeyCode::M;
		map[GLFW_KEY_N] = Pitaya::Input::KeyCode::N;
		map[GLFW_KEY_O] = Pitaya::Input::KeyCode::O;
		map[GLFW_KEY_P] = Pitaya::Input::KeyCode::P;
		map[GLFW_KEY_Q] = Pitaya::Input::KeyCode::Q;
		map[GLFW_KEY_R] = Pitaya::Input::KeyCode::R;
		map[GLFW_KEY_S] = Pitaya::Input::KeyCode::S;
		map[GLFW_KEY_T] = Pitaya::Input::KeyCode::T;
		map[GLFW_KEY_U] = Pitaya::Input::KeyCode::U;
		map[GLFW_KEY_V] = Pitaya::Input::KeyCode::V;
		map[GLFW_KEY_W] = Pitaya::Input::KeyCode::W;
		map[GLFW_KEY_X] = Pitaya::Input::KeyCode::X;
		map[GLFW_KEY_Y] = Pitaya::Input::KeyCode::Y;
		map[GLFW_KEY_Z] = Pitaya::Input::KeyCode::Z;

		map[GLFW_KEY_SPACE] = Pitaya::Input::KeyCode::Space;
		map[GLFW_KEY_SEMICOLON] = Pitaya::Input::KeyCode::Semicolon;
		map[GLFW_KEY_EQUAL] = Pitaya::Input::KeyCode::Equal;
		map[GLFW_KEY_LEFT_BRACKET] = Pitaya::Input::KeyCode::LeftBracket;
		map[GLFW_KEY_RIGHT_BRACKET] = Pitaya::Input::KeyCode::RightBracket;
		map[GLFW_KEY_COMMA] = Pitaya::Input::KeyCode::Comma;

		map[GLFW_KEY_LEFT_SHIFT] = Pitaya::Input::KeyCode::LeftShit;
		map[GLFW_KEY_LEFT_CONTROL] = Pitaya::Input::KeyCode::LeftControl;
		map[GLFW_KEY_LEFT_ALT] = Pitaya::Input::KeyCode::LeftAlt;
		map[GLFW_KEY_LEFT_SUPER] = Pitaya::Input::KeyCode::LeftSuper;
		map[GLFW_KEY_RIGHT_SHIFT] = Pitaya::Input::KeyCode::RightShit;
		map[GLFW_KEY_RIGHT_CONTROL] = Pitaya::Input::KeyCode::RightControl;
		map[GLFW_KEY_RIGHT_ALT] = Pitaya::Input::KeyCode::RightAlt;
		map[GLFW_KEY_RIGHT_SUPER] = Pitaya::Input::KeyCode::RightSuper;

		map[GLFW_KEY_ESCAPE] = Pitaya::Input::KeyCode::Escape;
		map[GLFW_KEY_ENTER] = Pitaya::Input::KeyCode::Enter;
		map[GLFW_KEY_TAB] = Pitaya::Input::KeyCode::Tab;
		map[GLFW_KEY_BACKSPACE] = Pitaya::Input::KeyCode::Backspace;
		map[GLFW_KEY_INSERT] = Pitaya::Input::KeyCode::Insert;
		map[GLFW_KEY_DELETE] = Pitaya::Input::KeyCode::Delete;
		map[GLFW_KEY_RIGHT] = Pitaya::Input::KeyCode::Right;
		map[GLFW_KEY_LEFT] = Pitaya::Input::KeyCode::Left;
		map[GLFW_KEY_DOWN] = Pitaya::Input::KeyCode::Down;
		map[GLFW_KEY_UP] = Pitaya::Input::KeyCode::UP;

		map[GLFW_KEY_PAGE_UP] = Pitaya::Input::KeyCode::PageUp;
		map[GLFW_KEY_PAGE_DOWN] = Pitaya::Input::KeyCode::PageDown;
		map[GLFW_KEY_HOME] = Pitaya::Input::KeyCode::Home;
		map[GLFW_KEY_END] = Pitaya::Input::KeyCode::End;
		map[GLFW_KEY_CAPS_LOCK] = Pitaya::Input::KeyCode::CapsLock;

		map[GLFW_KEY_F1] = Pitaya::Input::KeyCode::F1;
		map[GLFW_KEY_F2] = Pitaya::Input::KeyCode::F2;
		map[GLFW_KEY_F3] = Pitaya::Input::KeyCode::F3;
		map[GLFW_KEY_F4] = Pitaya::Input::KeyCode::F4;
		map[GLFW_KEY_F5] = Pitaya::Input::KeyCode::F5;
		map[GLFW_KEY_F6] = Pitaya::Input::KeyCode::F6;
		map[GLFW_KEY_F7] = Pitaya::Input::KeyCode::F7;
		map[GLFW_KEY_F8] = Pitaya::Input::KeyCode::F8;
		map[GLFW_KEY_F9] = Pitaya::Input::KeyCode::F9;
		map[GLFW_KEY_F10] = Pitaya::Input::KeyCode::F10;
		map[GLFW_KEY_F11] = Pitaya::Input::KeyCode::F11;
		map[GLFW_KEY_F12] = Pitaya::Input::KeyCode::F12;
	}
}

bool Pitaya::Window::Window::Initialize(int width, int height, const char* title)
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
	auto window = glfwCreateWindow(width, height, title, NULL, NULL);
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

	SetGLFWWindowStyle(window);
	glfwShowWindow(window);
	FillGLFWWindowKeyMap(map);

	backendStorage.Cast<GLFWWindowContext>().Window = window;
	return true;
}
void Pitaya::Window::Window::Release()
{
	auto& window = backendStorage.Cast<GLFWWindowContext>().Window;
	glfwDestroyWindow(window);	//销毁窗口
	glfwTerminate();			//卸载库
	window = nullptr;
}
bool Pitaya::Window::Window::IsClose() const
{
	return static_cast<bool>(glfwWindowShouldClose(backendStorage.Cast<GLFWWindowContext>().Window));
}
void Pitaya::Window::Window::PollEvents(Pitaya::Core::PassKey<Pitaya::Engine::Engine>) const
{
	glfwPollEvents();
}
void Pitaya::Window::Window::CloseWindow(Pitaya::Core::PassKey<Pitaya::Engine::Engine>) const
{
	glfwSetWindowShouldClose(backendStorage.Cast<GLFWWindowContext>().Window, GLFW_TRUE);
}
void* Pitaya::Window::Window::GetNativeWindow() const
{
	return backendStorage.Cast<GLFWWindowContext>().Window;
}
glm::uvec2 Pitaya::Window::Window::GetWindowSize() const
{
	int width, height;
	glfwGetFramebufferSize(backendStorage.Cast<GLFWWindowContext>().Window, &width, &height);
	return { width, height };
}
#endif