#pragma once

#include<Context/Context.h>
#include<Profiler/Common/FrameState.h>
#include<stdexcept>

namespace Pitaya::Engine
{
	template<>
	struct FuncTable<Pitaya::Profiler::FrameMonitor>
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
			if (!OnWriteTimeState) { throw std::runtime_error("Delegates miss [Profiler::WriteTimeState] Function!"); }
			if (!OnWriteRenderState) { throw std::runtime_error("Delegates miss [Profiler::WriteRenderStat] Function!"); }
			if (!OnGetTimeState) { throw std::runtime_error("Delegates miss [Profiler::GetTimeState] Function!"); }
			if (!OnGetRenderState) { throw std::runtime_error("Delegates miss [Profiler::GetRenderState] Function!"); }
			return true;
		}
		inline void UnRegister() noexcept
		{
			OnWriteTimeState = nullptr;
			OnWriteRenderState = nullptr;
			OnGetTimeState = nullptr;
			OnGetRenderState = nullptr;
		}

	public:
		inline void InvokeOnWriteTimeState(Pitaya::Profiler::TimeState timeState) noexcept
		{
			OnWriteTimeState(timeState);
		}
		inline void InvokeOnWriteRenderState(Pitaya::Profiler::RenderState renderState) noexcept
		{
			OnWriteRenderState(renderState);
		}
		inline Pitaya::Profiler::TimeState InvokeOnGetTimeState() noexcept
		{
			return OnGetTimeState();
		}
		inline Pitaya::Profiler::RenderState InvokeOnGetRenderState() noexcept
		{
			return OnGetRenderState();
		}
	private:
		void (*OnWriteTimeState)(Pitaya::Profiler::TimeState) noexcept = nullptr;
		void (*OnWriteRenderState)(Pitaya::Profiler::RenderState) noexcept = nullptr;
		Pitaya::Profiler::TimeState (*OnGetTimeState)() noexcept = nullptr;
		Pitaya::Profiler::RenderState (*OnGetRenderState)() noexcept = nullptr;
	};
}

namespace Pitaya::Profiler
{
	inline void WriteTimeState(Pitaya::Profiler::TimeState timeState) noexcept
	{
		Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::Profiler::FrameMonitor>().InvokeOnWriteTimeState(timeState);
	}
	inline void WriteRenderState(Pitaya::Profiler::RenderState renderState) noexcept
	{
		Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::Profiler::FrameMonitor>().InvokeOnWriteRenderState(renderState);
	}
	inline Pitaya::Profiler::TimeState GetTimeState() noexcept
	{
		return Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::Profiler::FrameMonitor>().InvokeOnGetTimeState();
	}
	inline Pitaya::Profiler::RenderState GetRenderState() noexcept
	{
		return Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::Profiler::FrameMonitor>().InvokeOnGetRenderState();
	}
}
