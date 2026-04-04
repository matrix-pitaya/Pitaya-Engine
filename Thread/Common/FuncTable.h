#pragma once

#include<Context/Context.h>
#include<Core/Thread/Thread.h>
#include<stdexcept>
#include<string>

namespace Pitaya::Engine
{
	template<>
	struct FuncTable<Pitaya::Thread::ThreadTracker>
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

		inline bool Check() const
		{
			if (!OnRegisterThread) { throw std::runtime_error("FuncTable miss [Thread::RegisterThread] Function!"); }
			if (!OnUnregisterThread) { throw std::runtime_error("FuncTable miss [Thread::UnregisterThread] Function!"); }
			if (!OnGetThreadName) { throw std::runtime_error("FuncTable miss [Thread::GetThreadName] Function!"); }
			if (!OnGetThreadIsRunning) { throw std::runtime_error("FuncTable miss [Thread::GetThreadIsRunning] Function!"); }
			return true;
		}
		inline void UnRegister() noexcept
		{
			OnRegisterThread = nullptr;
			OnUnregisterThread = nullptr;
			OnGetThreadName = nullptr;
			OnGetThreadIsRunning = nullptr;
		}

	public:
		inline Pitaya::Core::Thread::Identifier InvokeOnRegisterThread(std::string_view name, void(*Thread)(void*, void*), void* bootstraper, void* args)
		{
			return OnRegisterThread(name, Thread, bootstraper, args);
		}
		inline bool InvokeOnUnregisterThread(Pitaya::Core::Thread::Identifier id) noexcept
		{
			return OnUnregisterThread(id);
		}
		inline std::string InvokeOnGetThreadName(Pitaya::Core::Thread::Identifier id) noexcept
		{
			return OnGetThreadName(id);
		}
		inline bool InvokeOnGetThreadIsRunning(Pitaya::Core::Thread::Identifier id) noexcept
		{
			return OnGetThreadIsRunning(id);
		}

	private:
		Pitaya::Core::Thread::Identifier (ENGINE_CALL *OnRegisterThread)(std::string_view, void(*)(void*, void*), void*, void*) = nullptr;
		bool (ENGINE_CALL *OnUnregisterThread)(Pitaya::Core::Thread::Identifier) noexcept = nullptr;
		std::string (ENGINE_CALL *OnGetThreadName)(Pitaya::Core::Thread::Identifier) noexcept = nullptr;
		bool (ENGINE_CALL *OnGetThreadIsRunning)(Pitaya::Core::Thread::Identifier) noexcept = nullptr;
	};
}

namespace Pitaya::Thread
{
	inline Pitaya::Core::Thread::Identifier RegisterThread(std::string_view name, void(*Thread)(void*, void*), void* bootstraper, void* args)
	{
		return Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::Thread::ThreadTracker>().InvokeOnRegisterThread(name, Thread, bootstraper, args);
	}
	inline bool UnregisterThread(Pitaya::Core::Thread::Identifier id) noexcept
	{
		return Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::Thread::ThreadTracker>().InvokeOnUnregisterThread(id);
	}
	inline std::string GetThreadName(Pitaya::Core::Thread::Identifier id) noexcept
	{
		return Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::Thread::ThreadTracker>().InvokeOnGetThreadName(id);
	}
	inline bool GetThreadIsRunning(Pitaya::Core::Thread::Identifier id) noexcept
	{
		return Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::Thread::ThreadTracker>().InvokeOnGetThreadIsRunning(id);
	}
}