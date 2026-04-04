#include <Core/Utils/System/Console.h>

#include <iostream>
#include <cstdio>
#include <windows.h>

#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#endif

namespace
{
    static FILE* ConsoleOutStream = nullptr;
    static FILE* ConsoleInStream = nullptr;
    static FILE* ConsoleErrStream = nullptr;
    static bool IsConsoleAllocated = false;
}

bool Pitaya::Core::OpenConsole()
{
    //如果之前已经创建过控制台 则不再重新创建 直接显示
    if (IsConsoleAllocated)
    {
        HWND consoleWindow = GetConsoleWindow();
        if (consoleWindow != nullptr)
        {
            ShowWindow(consoleWindow, SW_SHOW); //仅仅是显示窗口
        }
        return true;
    }

    //分配一个控制台窗口 整个引擎生命周期只做一次
    AllocConsole();
    IsConsoleAllocated = true;

    //重定向 C 语言流
    freopen_s(&ConsoleOutStream, "CONOUT$", "w", stdout);
    freopen_s(&ConsoleErrStream, "CONOUT$", "w", stderr);
    freopen_s(&ConsoleInStream, "CONIN$", "r", stdin);

    //重置 C++ 流状态
    std::cout.clear();
    std::clog.clear();
    std::cerr.clear();
    std::cin.clear();

    //设置 UTF-8 编码，防止中文乱码
    SetConsoleOutputCP(CP_UTF8);

    //开启控制台的 ANSI 颜色支持
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut != INVALID_HANDLE_VALUE)
    {
        DWORD dwMode = 0;
        if (GetConsoleMode(hOut, &dwMode))
        {
            //给当前控制台模式加上虚拟终端解析能力
            dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
            SetConsoleMode(hOut, dwMode);
        }
    }

    //禁用控制台的快速编辑模式 防止鼠标点击导致引擎卡死
    HANDLE hIn = GetStdHandle(STD_INPUT_HANDLE);
    if (hIn != INVALID_HANDLE_VALUE)
    {
        DWORD dwInMode = 0;
        if (GetConsoleMode(hIn, &dwInMode))
        {
            //去除快速编辑模式，必须配合 ENABLE_EXTENDED_FLAGS 使用
            dwInMode &= ~ENABLE_QUICK_EDIT_MODE;
            dwInMode |= ENABLE_EXTENDED_FLAGS;
            SetConsoleMode(hIn, dwInMode);
        }
    }

    //禁用右上角X按钮 设置标题
    HWND consoleWindow = GetConsoleWindow();
    if (consoleWindow != nullptr)
    {
        HMENU hMenu = GetSystemMenu(consoleWindow, FALSE);
        if (hMenu != nullptr)
        {
            DeleteMenu(hMenu, SC_CLOSE, MF_BYCOMMAND);
        }
        SetConsoleTitleA("Pitaya Engine Console");
    }
    return true;
}

bool Pitaya::Core::CloseConsole()
{
    HWND consoleWindow = GetConsoleWindow();
    if (consoleWindow == nullptr) { return true; }

    // 安全隐藏窗口，绝不释放资源，防止引擎后台 cout 时崩溃
    ShowWindow(consoleWindow, SW_HIDE);
    return true;
}
