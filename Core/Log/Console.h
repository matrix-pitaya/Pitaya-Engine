#pragma once

#include"../Enum/Enum.h"

#include<cstdarg>
#include<cstdio>

#define DEBUG_VERSION

namespace Pitaya::Core::Console
{
    inline void Print(const char* fmt, ...)
    {
#ifdef DEBUG_VERSION
        char buffer[1024] = {};
        va_list args;
        va_start(args, fmt);
        vsnprintf(buffer, sizeof(buffer), fmt, args);
        va_end(args);
        printf("\033[0m%s\033[0m\n", buffer);
#endif
    }
    inline void Print(Core::Color color, const char* fmt, ...)
    {
#ifdef DEBUG_VERSION
        const char* colorCode = "\033[0m";
        switch (color)
        {
            case Core::Color::White:    colorCode = "\033[37m"; break;
            case Core::Color::Black:    colorCode = "\033[30m"; break;
            case Core::Color::Green:    colorCode = "\033[32m"; break;
            case Core::Color::Red:      colorCode = "\033[31m"; break;
            case Core::Color::Yellow:   colorCode = "\033[33m"; break;
            case Core::Color::Blue:     colorCode = "\033[34m"; break;
            default:                    colorCode = "\033[0m";  break;
        }
        char buffer[1024] = {};
        va_list args;
        va_start(args, fmt);
        vsnprintf(buffer, sizeof(buffer), fmt, args);
        va_end(args);
        printf("%s%s\033[0m\n", colorCode, buffer);
#endif
    }
}
