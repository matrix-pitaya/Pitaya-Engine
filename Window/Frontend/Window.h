#pragma once

#include<Core/Allocate/Allocate.h>
#include<Core/Storage/Storage.h>
#include<Core/PassKey/PassKey.h>
#include<Context/Common/Module.h>
#include<Input/Common/KeyCode.h>
#include<Window/Common/Platform.h>
#include<Window/Frontend/Window.h>

#include<glm.hpp>

namespace Pitaya::Window
{
	class Window
	{
	public:
		struct Factory
		{
			friend class Pitaya::Engine::Module<Window>;
		private:
			static Window* Create(Pitaya::Window::Platform);
			static void Destroy(Window* renderer)
			{
				PITAYA_DELETE(renderer);
			}
		};
		struct Lifecycle
		{
			friend class Pitaya::Engine::Module<Window>;
		private:
			static bool Initialize(Window* window, int width, int height, const char* title)
			{
				return window->Initialize(width, height, title);
			}
			static void Release(Window* window)
			{
				window->Release();
			}
		};

	protected:
		Window() = default;
		virtual ~Window() = default;

	public:
		Window(const Window&) = delete;
		Window& operator=(const Window&) = delete;
		Window(Window&&) = delete;
		Window& operator=(Window&&) = delete;

	protected:
		virtual bool Initialize(int width, int height, const char* title) = 0;
		virtual void Release() = 0;

	public:
		virtual void PollEvents(Pitaya::Core::PassKey<Pitaya::Engine::Engine>) const = 0;
		virtual	void CloseWindow(Pitaya::Core::PassKey<Pitaya::Engine::Engine>) const = 0;

	public:
		virtual bool IsClose() const = 0;
		virtual void* GetNativeWindow() const = 0;

	public:
		virtual glm::uvec2 GetWindowSize() const = 0;

	public:
		inline Pitaya::Input::KeyCode ToKeyCode(int key) const noexcept
		{
			return (key < 0 || key >= 512) ? Pitaya::Input::KeyCode::Unknown : map[key];
		}

	protected:
		Pitaya::Input::KeyCode map[512] = {};
		Pitaya::Core::Storage<8> backendStorage;
	};
}
