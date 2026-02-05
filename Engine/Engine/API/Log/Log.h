#pragma once

#include<string>

namespace Pitaya::Engine::Log
{
	void LogInfo(const std::string& message) noexcept;
	void LogDebug(const std::string& message) noexcept;
	void LogWarning(const std::string& message) noexcept;
	void LogError(const std::string& message) noexcept;
}