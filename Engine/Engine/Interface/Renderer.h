#pragma once

#include"Window.h"

namespace Pitaya::Engine::Interface
{
	class Renderer
	{
	public:
		Renderer() = default;
		virtual ~Renderer() = default;
		Renderer(const Renderer&) = delete;
		Renderer& operator=(const Renderer&) = delete;
		Renderer(Renderer&&) = delete;
		Renderer& operator=(Renderer&&) = delete;

	public:
		virtual bool Initialize() = 0;
		virtual void Release() = 0;
		virtual void Render() = 0;

		inline void Bind(Pitaya::Engine::Interface::Window* window) noexcept
		{
			this->window = window;
		}

	protected:
		Pitaya::Engine::Interface::Window* window = nullptr;
		bool isInitialized = false;
		bool isReleased = false;
	};
}