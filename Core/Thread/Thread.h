#pragma once

#include<Engine/API/def.h>

#include<thread>
#include<chrono>

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
        bool JoinWaitForMilliseconds(std::chrono::milliseconds timeout) noexcept;
        inline void Detach() noexcept
        {
            if (thread.joinable())
            {
                thread.detach();
            }
        }
        
        bool IsRunning() noexcept;
        Identifier GetThreadId() noexcept;
        inline std::thread::native_handle_type GetNativeHandle() noexcept
        {
            return thread.native_handle();
        }
        
    public:
        ENGINE_API static Identifier ENGINE_CALL GetCurrentThreadId() noexcept;

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
