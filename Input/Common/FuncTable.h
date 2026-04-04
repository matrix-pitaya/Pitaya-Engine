#pragma once

#include<Context/Context.h>
#include<Input/Common/KeyCode.h>
#include<stdexcept>

namespace Pitaya::Engine
{
	template<>
	struct FuncTable<Pitaya::Input::InputMonitor>
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
			if (!OnGetKeyDown) { throw std::runtime_error("FuncTable miss [Input::GetKeyDown] Function!"); }
			if (!OnGetKeyPressed) { throw std::runtime_error("FuncTable miss [Input::GetKeyPressed] Function!"); }
			if (!OnGetKeyReleased) { throw std::runtime_error("FuncTable miss [Input::GetKeyReleased] Function!"); }
			return true;
		}
		inline void UnRegister() noexcept
		{
			OnGetKeyDown = nullptr;
			OnGetKeyPressed = nullptr;
			OnGetKeyReleased = nullptr;
		}

	public:
		bool InvokeOnGetKeyDown(Pitaya::Input::KeyCode keyCode) noexcept
		{
			return OnGetKeyDown(keyCode);
		}
		bool InvokeOnGetKeyPressed(Pitaya::Input::KeyCode keyCode) noexcept
		{
			return OnGetKeyPressed(keyCode);
		}
		bool InvokeOnGetKeyReleased(Pitaya::Input::KeyCode keyCode) noexcept
		{
			return OnGetKeyReleased(keyCode);
		}

	private:
		bool (ENGINE_CALL *OnGetKeyDown)(Pitaya::Input::KeyCode) noexcept = nullptr;
		bool (ENGINE_CALL *OnGetKeyPressed)(Pitaya::Input::KeyCode) noexcept = nullptr;
		bool (ENGINE_CALL *OnGetKeyReleased)(Pitaya::Input::KeyCode) noexcept = nullptr;
	};
}

namespace Pitaya::Input
{
	inline bool GetKeyDown(Pitaya::Input::KeyCode keyCode) noexcept
	{
		return Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::Input::InputMonitor>().InvokeOnGetKeyDown(keyCode);
	}
	inline bool GetKeyPressed(Pitaya::Input::KeyCode keyCode) noexcept
	{
		return Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::Input::InputMonitor>().InvokeOnGetKeyPressed(keyCode);
	}
	inline bool GetKeyReleased(Pitaya::Input::KeyCode keyCode) noexcept
	{
		return Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::Input::InputMonitor>().InvokeOnGetKeyReleased(keyCode);
	}
}