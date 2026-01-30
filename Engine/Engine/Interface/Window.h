#pragma once

#include"Define/Define.h"
#include"Enum/Enum.h"

namespace Pitaya::Engine::Interface
{
	class Window
	{
		DECLARE_INTERFACE(Window)

	public:
		virtual bool Initialize(int width, int height, const char* title) = 0;
		virtual void Release() = 0;
		virtual bool GetKeyDown(Pitaya::Engine::Input::KeyCode keyCode) const = 0;
		virtual bool IsClose() const = 0;
		virtual void ClearFrameBuffer() const = 0;
		virtual void PollEvents() const = 0;
		virtual	void SwapBuffer() const = 0;
		virtual	void CloseWindow() const = 0;
		virtual void* GetNativeWindow() const = 0;

	protected:
		bool isInitialized = false;
		bool isReleased = false;
	};
}