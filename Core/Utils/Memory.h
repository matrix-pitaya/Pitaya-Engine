#pragma once

#include<string>
#include<mimalloc.h>

namespace Pitaya::Core
{
	inline std::string GetMemoryState() noexcept
	{
		std::string message;
		mi_stats_print_out([](const char* message, void* arg)
			{
				if (message == nullptr || arg == nullptr) { return; }
				std::string* str = static_cast<std::string*>(arg);
				str->append(message);
			}, &message);
		return message;
	}
}