#pragma once

#include"Define/Define.h"

#include"Window.h"

namespace Pitaya::Engine::Interface
{
	class Renderer
	{
		DECLARE_INTERFACE(Renderer)

	public:
		virtual bool Initialize() = 0;
		virtual void Release() = 0;
		virtual void Render() = 0;

		inline void Bind(Pitaya::Engine::Interface::Window* window)
		{
			this->window = window;
		}

	protected:
		Pitaya::Engine::Interface::Window* window = nullptr;
		bool isInitialized = false;
		bool isReleased = false;
	};
}