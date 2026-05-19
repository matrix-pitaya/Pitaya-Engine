#pragma once

#include<Context/Context.h>
#include<Log/Common/LogLevel.h>
#include<format>
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
	template<typename... Args>
	inline void Write(Pitaya::Log::LogLevel level, std::format_string<Args...> fmt, Args&&... args) noexcept
	{
		char buffer[1024] = {};
		auto result = std::format_to_n(buffer, sizeof(buffer), fmt, std::forward<Args>(args)...);
		if (static_cast<size_t>(result.out - buffer) < sizeof(buffer))
		{
			Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::Log::Logger>().InvokeOnLog(level, std::string_view(buffer, result.out));
		}
		else
		{
			auto str = std::format(fmt, std::forward<Args>(args)...);
			Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::Log::Logger>().InvokeOnLog(level, str);
		}
	}

	inline void Info(std::string_view info) noexcept
	{
		Write(Pitaya::Log::LogLevel::Info, info);
	}
	template<typename... Args>
	inline void Info(std::format_string<Args...> fmt, Args&&... args) noexcept
	{
		Write(Pitaya::Log::LogLevel::Info, fmt, std::forward<Args>(args)...);
	}

	inline void Debug(std::string_view info) noexcept
	{
		Write(Pitaya::Log::LogLevel::Debug, info);
	}
	template<typename... Args>
	inline void Debug(std::format_string<Args...> fmt, Args&&... args) noexcept
	{
		Write(Pitaya::Log::LogLevel::Debug, fmt, std::forward<Args>(args)...);
	}

	inline void Warning(std::string_view info) noexcept
	{
		Write(Pitaya::Log::LogLevel::Warning, info);
	}
	template<typename... Args>
	inline void Warning(std::format_string<Args...> fmt, Args&&... args) noexcept
	{
		Write(Pitaya::Log::LogLevel::Warning, fmt, std::forward<Args>(args)...);
	}

	inline void Error(std::string_view info) noexcept
	{
		Write(Pitaya::Log::LogLevel::Error, info);
	}
	template<typename... Args>
	inline void Error(std::format_string<Args...> fmt, Args&&... args) noexcept
	{
		Write(Pitaya::Log::LogLevel::Error, fmt, std::forward<Args>(args)...);
	}
}
