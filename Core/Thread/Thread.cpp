#include<Core/Thread/Thread.h>

#include<windows.h>

bool Pitaya::Core::Thread::JoinWaitForMilliseconds(std::chrono::milliseconds timeout) noexcept
{
    if (!thread.joinable()) { return true; }

    const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(timeout).count();
    const DWORD dw_ms = static_cast<DWORD>(ms < 0 ? 0 : ms);
    const HANDLE hThread = static_cast<HANDLE>(thread.native_handle());
    const DWORD ret = WaitForSingleObject(hThread, dw_ms);

    if (ret == WAIT_OBJECT_0)
    {
        thread.join();
        return true;
    }

    ::TerminateThread(hThread, 0xFFFFFFFF);
    thread.join();
    return false;
}
bool Pitaya::Core::Thread::IsRunning() noexcept
{
    if (!thread.joinable()) { return false; }

    const HANDLE hThread = static_cast<HANDLE>(thread.native_handle());
    DWORD exitCode = 0;
    return GetExitCodeThread(hThread, &exitCode) && (exitCode == STILL_ACTIVE);
}
Pitaya::Core::Thread::Identifier Pitaya::Core::Thread::GetThreadId() noexcept
{
    if (!thread.joinable()) { return 0; }

    return static_cast<uint64_t>(::GetThreadId(static_cast<HANDLE>(thread.native_handle())));
}
Pitaya::Core::Thread::Identifier Pitaya::Core::Thread::GetCurrentThreadId() noexcept
{
    return static_cast<uint64_t>(::GetCurrentThreadId());
}
