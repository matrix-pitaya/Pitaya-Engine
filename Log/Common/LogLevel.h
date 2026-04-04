#pragma once

#include<cstdint>
#include<string>

namespace Pitaya::Log
{
	enum class LogLevel : uint8_t
	{
		Info,
		Warning,
		Error,
		Debug
	};

	inline std::string_view ToString(LogLevel level) noexcept
	{
		switch (level)
		{
			case Pitaya::Log::LogLevel::Info:		return "Info";
			case Pitaya::Log::LogLevel::Debug:		return "Debug";
			case Pitaya::Log::LogLevel::Warning:	return "Warning";
			case Pitaya::Log::LogLevel::Error:		return "Error";
			default:								return "";
		}
	}
}
