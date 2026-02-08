#pragma once

#include<Color/Color.h>

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
        int r = static_cast<int>(color.r * 255.0f);
        int g = static_cast<int>(color.g * 255.0f);
        int b = static_cast<int>(color.b * 255.0f);

        char colorCode[32] = {};
        snprintf(colorCode, sizeof(colorCode), "\033[38;2;%d;%d;%dm", r, g, b);

        char buffer[1024] = {};
        va_list args;
        va_start(args, fmt);
        vsnprintf(buffer, sizeof(buffer), fmt, args);
        va_end(args);

        printf("%s%s\033[0m\n", colorCode, buffer);
#endif
    }

    void PopupMessageBox(const char* title, const char* message);
}
