#pragma once

#if defined(PITAYA_PLATFORM_WINDOWS)
#define NOMINMAX
#include<windows.h>
#endif

#include<cstdio>
#include<cstdlib>

namespace Pitaya::Core
{
	[[noreturn]] inline void _OnCheckFail(const char* expr, const char* file, int line) noexcept
	{
		char buf[1024] = {};
		std::snprintf(buf, sizeof(buf), "[CHECK FAILED] %s\n  at %s:%d\n", expr, file, line);

		std::fputs(buf, stderr);
		std::fflush(stderr);

#if defined(PITAYA_PLATFORM_WINDOWS)
		OutputDebugStringA(buf);
		MessageBoxA(NULL, buf, "Pitaya Check Failed", MB_OK | MB_ICONERROR | MB_TOPMOST);
#endif

		std::abort();
	}
}

#define PITAYA_CHECK(cond)											\
	do																\
	{																\
		if (!(cond))												\
		{															\
			Pitaya::Core::_OnCheckFail(#cond, __FILE__, __LINE__);	\
		}															\
	} while (false);
