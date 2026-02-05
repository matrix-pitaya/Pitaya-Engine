#pragma once

#include"Window.h"

namespace Pitaya::Engine
{
	class Engine;
}

namespace Pitaya::Engine::Interface
{
	class Renderer
	{
		friend class Pitaya::Engine::Engine;
	protected:
		Renderer() = default;
		virtual ~Renderer() = default;

	public:
		Renderer(const Renderer&) = delete;
		Renderer& operator=(const Renderer&) = delete;
		Renderer(Renderer&&) = delete;
		Renderer& operator=(Renderer&&) = delete;

	protected:
		virtual bool Initialize() = 0;
		virtual void Release() = 0;
		virtual void Render() = 0;

	protected:
		inline void Bind(Pitaya::Engine::Interface::Window* window) noexcept
		{
			this->window = window;
		}

	protected:
		Pitaya::Engine::Interface::Window* window = nullptr;
	};
}