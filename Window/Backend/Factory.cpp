#include<Window/Frontend/Window.h>
#include<Window/Backend/GLFW/GLFWWindow.h>

Pitaya::Window::Window* Pitaya::Window::Window::Factory::Create(Pitaya::Window::Platform platform)
{
	switch (platform)
	{
		case Pitaya::Window::Platform::GLFW:
			return PITAYA_NEW(Pitaya::Window::GLFWWindow);

		case Pitaya::Window::Platform::Unknown:
			throw std::exception("Unknown Window WindowPlatform");

		default:
			throw std::exception("Unknown Window WindowPlatform");
	}
}