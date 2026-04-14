#pragma once

#include<thread>
#include<chrono>

#define NOMINMAX
#include<windows.h>

namespace Pitaya::Core
{
    class Thread
    {
    public:
        struct Identifier
        {
            constexpr Identifier(uint64_t id = 0) noexcept
                : id(id) {}

            constexpr bool operator==(const Identifier& other) const noexcept
            {
                return id == other.id;
            }
            constexpr bool operator!=(const Identifier& other) const noexcept
            {
                return id != other.id;
            }
            constexpr bool operator<(const Identifier& other) const noexcept
            {
                return id < other.id;
            }

            explicit constexpr operator bool() const noexcept
            {
                return id != 0;
            }
            constexpr operator uint64_t() const noexcept
            {
                return id;
            }
            constexpr Identifier& operator=(uint64_t id) noexcept
            {
                this->id = id;
                return *this;
            }

        public:
            static const Identifier Invalid;

        private:
            uint64_t id = 0;
        };
    public:
        Thread() noexcept = default;
        ~Thread() noexcept
        {
            if (thread.joinable())
            {
                thread.detach();
            }
        }

        template <typename Func, typename... Args>
        explicit Thread(Func&& func, Args&&... args)
            : thread(std::forward<Func>(func), std::forward<Args>(args)...) {}

    public:
        Thread(const Thread&) = delete;
        Thread& operator=(const Thread&) = delete;
        Thread(Thread&&) noexcept = default;
        Thread& operator=(Thread&&) noexcept = default;

    public:
        inline bool Joinable() const noexcept
        {
            return thread.joinable();
        }
        inline bool Join() noexcept
        {
            if (thread.joinable())
            {
                thread.join();
            }
            return true;
        }
        inline bool JoinWaitForMilliseconds(std::chrono::milliseconds timeout) noexcept
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
        inline void Detach() noexcept
        {
            if (thread.joinable())
            {
                thread.detach();
            }
        }
        
        inline bool IsRunning() noexcept
        {
            if (!thread.joinable()) { return false; }
            const HANDLE hThread = static_cast<HANDLE>(thread.native_handle());
            DWORD exitCode = 0;
            return GetExitCodeThread(hThread, &exitCode) && (exitCode == STILL_ACTIVE);
        }
        inline Identifier GetThreadId() noexcept
        {
            return thread.joinable() ?
                static_cast<uint64_t>(::GetThreadId(static_cast<HANDLE>(thread.native_handle()))) :
                0;
        }
        inline std::thread::native_handle_type GetNativeHandle() noexcept
        {
            return thread.native_handle();
        }
        
    public:
        inline static Identifier GetCurrentThreadId() noexcept
        {
            return static_cast<uint64_t>(::GetCurrentThreadId());
        }

    private:
        std::thread thread;
    };

    inline constexpr const Thread::Identifier Thread::Identifier::Invalid = Identifier(0);
}

namespace std
{
    template <>
    struct hash<::Pitaya::Core::Thread::Identifier>
    {
        size_t operator()(const ::Pitaya::Core::Thread::Identifier& id) const noexcept
        {
            return std::hash<uint64_t>{}(id);
        }
    };
}
