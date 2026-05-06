#pragma once

#if defined(PITAYA_PLATFORM_WINDOWS)
#define NOMINMAX
#include<windows.h>
#endif

#include<stdlib.h>

namespace Pitaya::Core
{
	inline void PopMessageBox(const char* title, const char* info)
	{
#if defined(PITAYA_PLATFORM_WINDOWS)
		MessageBoxA(NULL, info, title, MB_OK);
#endif
	}
	inline void Terminate(int exitcode = 0)
	{
		exit(exitcode);
	}
    inline std::string LoadBuiltInRC(const char* resourceName, const char* moduleName = nullptr)
    {
#if defined(PITAYA_PLATFORM_WINDOWS)
        HMODULE hModule = GetModuleHandleA(moduleName);
        if (!hModule) { return ""; }

        HRSRC hRes = FindResourceA(hModule, resourceName, RT_RCDATA);
        if (!hRes) { return ""; }

        HGLOBAL hData = LoadResource(hModule, hRes);
        if (!hData) { return ""; }

        DWORD dataSize = SizeofResource(hModule, hRes);
        const char* dataPtr = static_cast<const char*>(LockResource(hData));
        if (!dataPtr || dataSize == 0) { return ""; }

        return std::string(dataPtr, dataSize);
#endif
    }
}