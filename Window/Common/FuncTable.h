#pragma once

#include<Context/Context.h>
#include<glm.hpp>

namespace Pitaya::Engine
{
	template<>
	struct FuncTable<Pitaya::Window::Window>
	{
		friend class Pitaya::Engine::Engine;
	private:
		FuncTable() = default;
		~FuncTable() = default;

	public:
		FuncTable(const FuncTable&) = delete;
		FuncTable& operator=(const FuncTable&) = delete;
		FuncTable(FuncTable&&) = delete;
		FuncTable& operator=(FuncTable&&) = delete;

	public:
		inline bool Verify() const
		{
			if (!OnGetNativeWindow) { throw std::runtime_error("FuncTable miss [Window::GetNativeWindow] Function!"); }
			if (!OnGetWindowSize) { throw std::runtime_error("FuncTable miss [Window::GetWindowSize] Function!"); }
			return true;
		}
		inline void Nullify() noexcept
		{
			OnGetNativeWindow = nullptr;
			OnGetWindowSize = nullptr;
		}

	public:
		inline void* InvokeOnGetNativeWindow() const
		{
			return OnGetNativeWindow();
		}
		inline glm::uvec2 InvokeOnGetWindowSize() const
		{
			return OnGetWindowSize();
		}

	private:
		void* (ENGINE_CALL *OnGetNativeWindow)() = nullptr;
		glm::uvec2 (ENGINE_CALL *OnGetWindowSize)() = nullptr;
	};
}


namespace Pitaya::Window
{
	inline void* GetNativeWindow()
	{
		return Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::Window::Window>().InvokeOnGetNativeWindow();
	}
	inline glm::uvec2 GetWindowSize()
	{
		return Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::Window::Window>().InvokeOnGetWindowSize();
	}
}
