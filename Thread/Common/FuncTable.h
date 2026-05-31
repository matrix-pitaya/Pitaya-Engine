#pragma once

#include<Context/Context.h>
#include<Core/Thread/Thread.h>
#include<Thread/Common/ThreadType.h>
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
			
		inline bool Verify() const
		{
			if (!OnRegisterThread) { throw std::runtime_error("FuncTable miss [Thread::RegisterThread] Function!"); }
			if (!OnUnregisterThread) { throw std::runtime_error("FuncTable miss [Thread::UnregisterThread] Function!"); }
			if (!OnGetThreadName) { throw std::runtime_error("FuncTable miss [Thread::GetThreadName] Function!"); }
			if (!OnGetThreadIsRunning) { throw std::runtime_error("FuncTable miss [Thread::GetThreadIsRunning] Function!"); }
			if (!OnGetIsInThread) { throw std::runtime_error("FuncTable miss [Thread::GetIsInThread] Function!"); }
			return true;
		}
		inline void Nullify() noexcept
		{
			OnRegisterThread = nullptr;
			OnUnregisterThread = nullptr;
			OnGetThreadName = nullptr;
			OnGetThreadIsRunning = nullptr;
			OnGetIsInThread = nullptr;
		}

	public:
		inline Pitaya::Core::Thread::Identifier InvokeOnRegisterThread(Pitaya::Thread::ThreadType type, std::string_view name, void(*Thread)(void*, void*), void* bootstraper, void* args)
		{
			return OnRegisterThread(type, name, Thread, bootstraper, args);
		}
		inline bool InvokeOnUnregisterThread(Pitaya::Core::Thread::Identifier id) noexcept
		{
			return OnUnregisterThread(id);
		}
		inline std::string_view InvokeOnGetThreadName(Pitaya::Core::Thread::Identifier id) noexcept
		{
			return OnGetThreadName(id);
		}
		inline bool InvokeOnGetThreadIsRunning(Pitaya::Core::Thread::Identifier id) noexcept
		{
			return OnGetThreadIsRunning(id);
		}
		inline bool InvokeOnGetIsInThread(Pitaya::Thread::ThreadType type) noexcept
		{
			return OnGetIsInThread(type);
		}

	private:
		Pitaya::Core::Thread::Identifier (ENGINE_CALL *OnRegisterThread)(Pitaya::Thread::ThreadType, std::string_view, void(*)(void*, void*), void*, void*) = nullptr;
		bool (ENGINE_CALL *OnUnregisterThread)(Pitaya::Core::Thread::Identifier) noexcept = nullptr;
		std::string_view(ENGINE_CALL *OnGetThreadName)(Pitaya::Core::Thread::Identifier) noexcept = nullptr;
		bool (ENGINE_CALL *OnGetThreadIsRunning)(Pitaya::Core::Thread::Identifier) noexcept = nullptr;
		bool (ENGINE_CALL *OnGetIsInThread)(Pitaya::Thread::ThreadType) noexcept = nullptr;
	};
}

namespace Pitaya::Thread
{
	inline Pitaya::Core::Thread::Identifier RegisterThread(Pitaya::Thread::ThreadType type, std::string_view name, void(*Thread)(void*, void*), void* bootstraper, void* args)
	{
		return Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::Thread::ThreadTracker>().InvokeOnRegisterThread(type, name, Thread, bootstraper, args);
	}
	inline bool UnregisterThread(Pitaya::Core::Thread::Identifier id) noexcept
	{
		return Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::Thread::ThreadTracker>().InvokeOnUnregisterThread(id);
	}
	inline std::string_view GetThreadName(Pitaya::Core::Thread::Identifier id) noexcept
	{
		return Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::Thread::ThreadTracker>().InvokeOnGetThreadName(id);
	}
	inline bool GetThreadIsRunning(Pitaya::Core::Thread::Identifier id) noexcept
	{
		return Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::Thread::ThreadTracker>().InvokeOnGetThreadIsRunning(id);
	}
	inline bool GetIsInThread(Pitaya::Thread::ThreadType type) noexcept
	{
		return Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::Thread::ThreadTracker>().InvokeOnGetIsInThread(type);
	}
}