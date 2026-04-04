#pragma once

#include<Context/Context.h>

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
		inline bool Check() const
		{
			if (!OnGetNativeWindow) { throw std::runtime_error("FuncTable miss [Window::GetNativeWindow] Function!"); }
			return true;
		}
		inline void UnRegister() noexcept
		{
			OnGetNativeWindow = nullptr;
		}

	public:
		inline void* InvokeOnGetNativeWindow() const
		{
			return OnGetNativeWindow();
		}

	private:
		void* (ENGINE_CALL *OnGetNativeWindow)() = nullptr;
	};
}


namespace Pitaya::Window
{
	inline void* GetNativeWindow()
	{
		return Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::Window::Window>().InvokeOnGetNativeWindow();
	}
}
