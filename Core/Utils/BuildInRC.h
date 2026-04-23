#pragma once

#include<string>

#define NOMINMAX
#include<windows.h>

namespace Pitaya::Core
{
    inline std::string LoadBuildInRC(int resourceID)
    {
        HMODULE hModule = GetModuleHandle(nullptr);
        if (!hModule) { return ""; }

        HRSRC hRes = FindResource(hModule, MAKEINTRESOURCE(resourceID), RT_RCDATA);
        if (!hRes) { return ""; }

        HGLOBAL hData = LoadResource(hModule, hRes);
        if (!hData) { return ""; }

        DWORD dataSize = SizeofResource(hModule, hRes);
        const char* dataPtr = static_cast<const char*>(LockResource(hData));
        if (!dataPtr || dataSize == 0) { return ""; }
        return std::string(dataPtr, dataSize);
    }
}
