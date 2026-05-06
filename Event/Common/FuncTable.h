#pragma once

#include<Context/Context.h>
#include<Event/Common/Event.h>
#include<stdexcept>
#include<functional>

namespace Pitaya::Engine
{
	template<>
	struct FuncTable<Pitaya::Event::EventDispatcher>
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
			if (!OnSubscribe) { throw std::runtime_error("FuncTable miss [Event::Subscribe] Function!"); }
			if (!OnUnSubscribe) { throw std::runtime_error("FuncTable miss [Event::UnSubscribe] Function!"); }
			if (!OnEmit) { throw std::runtime_error("FuncTable miss [Event::Emit] Function!"); }
			return true;
		}
		inline void Nullify() noexcept
		{
			OnSubscribe = nullptr;
			OnUnSubscribe = nullptr;
			OnEmit = nullptr;
		}

	public:
		inline Pitaya::Event::EventToken InvokeOnSubscribe(Pitaya::Event::EventType type, void (*OnCallBack)(void*, Pitaya::Event::Event), void* listener) noexcept
		{
			return OnSubscribe(type, OnCallBack, listener);
		}
		inline bool InvokeOnUnSubscribe(Pitaya::Event::EventToken eventToken) noexcept
		{
			return OnUnSubscribe(eventToken);
		}
		inline void InvokeOnEmit(Pitaya::Event::Event event) noexcept
		{
			OnEmit(event);
		}

	private:
		Pitaya::Event::EventToken (ENGINE_CALL *OnSubscribe)(Pitaya::Event::EventType, void (*)(void*, Pitaya::Event::Event), void*) noexcept = nullptr;
		bool (ENGINE_CALL *OnUnSubscribe)(Pitaya::Event::EventToken) noexcept = nullptr;
		void (ENGINE_CALL *OnEmit)(Pitaya::Event::Event) noexcept = nullptr;
	};
}

namespace Pitaya::Event
{
	inline Pitaya::Event::EventToken Subscribe(Pitaya::Event::EventType type, void (*OnCallBack)(void*, Pitaya::Event::Event), void* listener) noexcept
	{
		return Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::Event::EventDispatcher>().InvokeOnSubscribe(type, OnCallBack, listener);
	}
	inline bool UnSubscribe(Pitaya::Event::EventToken eventToken) noexcept
	{
		return Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::Event::EventDispatcher>().InvokeOnUnSubscribe(eventToken);
	}
	inline void Emit(Pitaya::Event::Event event) noexcept
	{
		Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::Event::EventDispatcher>().InvokeOnEmit(event);
	}
}
