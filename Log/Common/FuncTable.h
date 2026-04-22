#pragma once

#include<Context/Context.h>
#include<Log/Common/LogLevel.h>
#include<stdexcept>

namespace Pitaya::Engine
{
	template<>
	struct FuncTable<Pitaya::Log::Logger>
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
			if (!OnLog) { throw std::runtime_error("Delegates miss [Log::Log] Function!"); }
			return true;
		}
		inline void Nullify() noexcept
		{
			OnLog = nullptr;
		}

	public:
		inline void InvokeOnLog(Pitaya::Log::LogLevel level, std::string_view info) noexcept
		{
			OnLog(level, info);
		}

	private:
		void (ENGINE_CALL *OnLog)(Pitaya::Log::LogLevel, std::string_view) noexcept = nullptr;
	};
}

namespace Pitaya::Log
{
	inline void Write(Pitaya::Log::LogLevel level, std::string_view info) noexcept
	{
		Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::Log::Logger>().InvokeOnLog(level, info);
	}
	inline void Info(std::string_view info) noexcept
	{
		Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::Log::Logger>().InvokeOnLog(Pitaya::Log::LogLevel::Info, info);
	}
	inline void Debug(std::string_view info) noexcept
	{
		Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::Log::Logger>().InvokeOnLog(Pitaya::Log::LogLevel::Debug, info);
	}
	inline void Warning(std::string_view info) noexcept
	{
		Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::Log::Logger>().InvokeOnLog(Pitaya::Log::LogLevel::Warning, info);
	}
	inline void Error(std::string_view info) noexcept
	{
		Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::Log::Logger>().InvokeOnLog(Pitaya::Log::LogLevel::Error, info);
	}
}
