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
    
    struct BuiltRC
    {
        const void* data = nullptr;
        size_t size = 0;
    };
    inline BuiltRC LoadBuiltInRC(const char* resourceName, const char* moduleName = nullptr)
    {
#if defined(PITAYA_PLATFORM_WINDOWS)
        HMODULE hModule = GetModuleHandleA(moduleName);
        if (!hModule) { return {}; }

        HRSRC hRes = FindResourceA(hModule, resourceName, RT_RCDATA);
        if (!hRes) { return {}; }

        HGLOBAL hData = LoadResource(hModule, hRes);
        if (!hData) { return {}; }

        DWORD dataSize = SizeofResource(hModule, hRes);
        const void* dataPtr = LockResource(hData);
        if (!dataPtr || dataSize == 0) { return {}; }

        return { dataPtr, static_cast<size_t>(dataSize) };
#endif
    }
}