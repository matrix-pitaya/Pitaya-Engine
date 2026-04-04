#pragma once

#include<Context/Context.h>
#include<stdexcept>
#include<cstdint>

namespace Pitaya::Engine
{
	template<>
	struct FuncTable<Pitaya::Time::Chronometer>
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
			if (!Ondelta) { throw std::runtime_error("FuncTable miss [Time::delta] Function!"); }
			if (!OnFixdelta) { throw std::runtime_error("FuncTable miss [Time::Fixdelta] Function!"); }
			if (!OnUnscaledDelta) { throw std::runtime_error("FuncTable miss [Time::UnscaledDelta] Function!"); }
			if (!OnTimeScale) { throw std::runtime_error("FuncTable miss [Time::TimeScale] Function!"); }
			if (!OnFramerate) { throw std::runtime_error("FuncTable miss [Time::Framerate] Function!"); }
			if (!OnSeconds) { throw std::runtime_error("FuncTable miss [Time::Seconds] Function!"); }
			if (!OnMilliseconds) { throw std::runtime_error("FuncTable miss [Time::Milliseconds] Function!"); }
			return true;
		}
		inline void UnRegister() noexcept
		{
			Ondelta = nullptr;
			OnFixdelta = nullptr;
			OnUnscaledDelta = nullptr;
			OnTimeScale = nullptr;
			OnFramerate = nullptr;
			OnSeconds = nullptr;
			OnMilliseconds = nullptr;
		}

	public:
		inline float InvokeOndelta() const noexcept
		{
			return Ondelta();
		}
		inline float InvokeOnFixdelta() const noexcept
		{
			return OnFixdelta();
		}
		inline float InvokeOnUnscaledDelta() const noexcept
		{
			return OnUnscaledDelta();
		}
		inline float InvokeOnTimeScale() const noexcept
		{
			return OnTimeScale();
		}
		inline float InvokeOnFramerate() const noexcept
		{
			return OnFramerate();
		}
		inline double InvokeOnSeconds() const noexcept
		{
			return OnSeconds();
		}
		inline int64_t InvokeOnMilliseconds() const noexcept
		{
			return OnMilliseconds();
		}

	private:
		float (*Ondelta)() noexcept = nullptr;
		float (*OnFixdelta)() noexcept = nullptr;
		float (*OnUnscaledDelta)() noexcept = nullptr;
		float (*OnTimeScale)() noexcept = nullptr;
		float (*OnFramerate)() noexcept = nullptr;
		double (*OnSeconds)() noexcept = nullptr;
		int64_t(*OnMilliseconds)() noexcept = nullptr;
	};
}

namespace Pitaya::Time
{
	inline float delta() noexcept
	{
		return Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::Time::Chronometer>().InvokeOndelta();
	}
	inline float Fixdelta() noexcept
	{
		return Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::Time::Chronometer>().InvokeOnFixdelta();
	}
	inline float UnscaledDelta() noexcept
	{
		return Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::Time::Chronometer>().InvokeOnUnscaledDelta();
	}
	inline float TimeScale() noexcept
	{
		return Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::Time::Chronometer>().InvokeOnTimeScale();
	}
	inline float Framerate() noexcept
	{
		return Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::Time::Chronometer>().InvokeOnFramerate();
	}
	inline double Seconds() noexcept
	{
		return Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::Time::Chronometer>().InvokeOnSeconds();
	}
	inline int64_t Milliseconds() noexcept
	{
		return Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::Time::Chronometer>().InvokeOnMilliseconds();
	}
}