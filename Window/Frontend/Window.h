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
			static Window* Create()
			{
				return PITAYA_NEW(Window);
			}
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

	private:
		Window() = default;
		~Window() = default;

	public:
		Window(const Window&) = delete;
		Window& operator=(const Window&) = delete;
		Window(Window&&) = delete;
		Window& operator=(Window&&) = delete;

	private:
		bool Initialize(int width, int height, const char* title);
		void Release();

	public:
		void PollEvents(Pitaya::Core::PassKey<Pitaya::Engine::Engine>) const;
		void CloseWindow(Pitaya::Core::PassKey<Pitaya::Engine::Engine>) const;

	public:
		bool IsClose() const;
		void* GetNativeWindow() const;
		glm::uvec2 GetWindowSize() const;
		
	public:
		inline Pitaya::Input::KeyCode ToKeyCode(int key) const noexcept
		{
			return (key < 0 || key >= 512) ? Pitaya::Input::KeyCode::Unknown : map[key];
		}

	private:
		Pitaya::Input::KeyCode map[512] = {};
		Pitaya::Core::Storage<8> backendStorage;
	};
}
