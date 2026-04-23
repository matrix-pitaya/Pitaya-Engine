#pragma once

#include<Window/Frontend/Window.h>

#include<algorithm>

#include<GLFW/glfw3.h>

namespace Pitaya::Window
{
	class GLFWWindow : public Pitaya::Window::Window
	{
		friend class Pitaya::Window::Window;
	private:
		GLFWWindow() = default;
		~GLFWWindow() override = default;

	public:
		GLFWWindow(const GLFWWindow&) = delete;
		GLFWWindow& operator=(const GLFWWindow&) = delete;
		GLFWWindow(GLFWWindow&&) = delete;
		GLFWWindow& operator=(GLFWWindow&&) = delete;

	private:
		bool Initialize(int width, int height, const char* title) override;
		void Release() override;

	private:
		void SetWindowStyle();

	public:
		void PollEvents(Pitaya::Core::PassKey<Pitaya::Engine::Engine>) const override;
		void CloseWindow(Pitaya::Core::PassKey<Pitaya::Engine::Engine>) const override;

	public:
		bool IsClose() const override;
		void* GetNativeWindow() const override;

	private:
		inline void RegisterKeyMap() noexcept
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

	public:
		inline void ResetSize(int width, int height) noexcept
		{
			this->width = width;
			this->height = height;
		}

	private:
		static void FramebufferResetSizeCallback(GLFWwindow* window, int width, int height);
		static void MouseCursorMoveCallback(GLFWwindow* window, double xPosition, double yPosition);
		static void MouseScrollCallback(GLFWwindow* window, double xOffset, double yOffset);
		static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
		static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
		static void WindowCloseCallback(GLFWwindow* window);
		static void DropFileCallback(GLFWwindow* window, int count, const char** paths);

	private:
		GLFWwindow* window = nullptr;
	};
}
