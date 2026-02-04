#pragma once

#include"Enum/Enum.h"

#include"../../Engine/Engine/Interface/Window.h"

#include<GL/glew.h>
#include<GLFW/glfw3.h>

namespace Pitaya::Engine
{
	class Engine;
}

namespace Pitaya::Engine::Window
{
	class OpenGLWindow : public Pitaya::Engine::Interface::Window
	{
		friend class Pitaya::Engine::Engine;
	private:
		OpenGLWindow() = default;
		~OpenGLWindow() override { Release(); }

	public:
		OpenGLWindow(const OpenGLWindow&) = delete;
		OpenGLWindow& operator=(const OpenGLWindow&) = delete;
		OpenGLWindow(OpenGLWindow&&) = delete;
		OpenGLWindow& operator=(OpenGLWindow&&) = delete;

	public:
		bool Initialize(int width,int height, const char* title) override;
		void Release() override;
		bool GetKeyDown(Pitaya::Engine::Input::KeyCode keyCode) const override;
		bool IsClose() const override;
		void ClearFrameBuffer() const override;
		void PollEvents() const override;
		void SwapBuffer() const override;
		void CloseWindow() const override;
		void* GetNativeWindow() const override;

		void ResetSize(int width, int height);

	private:
		static void FramebufferResetSizeCallback(GLFWwindow* window, int width, int height);
		static void MouseCursorMoveCallback(GLFWwindow* window, double xPosition, double yPosition);
		static void MouseScrollCallback(GLFWwindow* window, double xOffset, double yOffset);
		static void KeyDownCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
		static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
		static void WindowCloseCallback(GLFWwindow* window);
		static void DropFileCallback(GLFWwindow* window, int count, const char** paths);

	private:
		GLFWwindow* window = nullptr;
		int width = 0;
		int height = 0;
	};
}