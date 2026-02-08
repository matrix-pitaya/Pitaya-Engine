#pragma once

#include<Enum/Enum.h>

#include<vector>

namespace Pitaya::Engine
{
	class Engine;
}

namespace Pitaya::Engine::Interface
{
	class Window
	{
		friend class Engine;
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

	protected:
		virtual void PollEvents() const = 0;
		virtual	void CloseWindow() const = 0;

	public:
		virtual bool IsClose() const = 0;
		virtual void* GetNativeWindow() const = 0;

	protected:
		inline Pitaya::Engine::Input::KeyCode IntToKeyCode(int key) const noexcept
		{
			return (key < 0 || key >= 512) ? Pitaya::Engine::Input::KeyCode::Unknown : map[key];
		}

	protected:
		std::vector<Pitaya::Engine::Input::KeyCode> map = std::vector<Pitaya::Engine::Input::KeyCode>(512, Pitaya::Engine::Input::KeyCode::Unknown);
	};
}