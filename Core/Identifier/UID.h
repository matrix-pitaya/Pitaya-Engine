#pragma once

#include<cstdint>
#include<atomic>
#include<functional>

namespace Pitaya::Core
{
    struct UID
    {
        explicit constexpr UID(uint64_t uid = 0) noexcept
            : value(uid) {}

        constexpr bool operator==(const UID& other) const noexcept
        { 
            return value == other.value;
        }
        constexpr bool operator!=(const UID& other) const noexcept
        { 
            return value != other.value;
        }
        constexpr bool operator<(const UID& other) const noexcept
        { 
            return value < other.value; 
        } 

        explicit constexpr operator bool() const noexcept
        {
            return IsValid();
        }

        inline constexpr bool IsValid() const noexcept
        { 
            return value != 0; 
        }
        inline constexpr void SetValue(uint64_t value) noexcept
        {
            this->value = value;
        }
        inline constexpr uint64_t GetValue() const noexcept
        {
            return value;
        }

    public:
        [[nodiscard]]
        static UID Next() noexcept
        { 
            static std::atomic<uint64_t> uid = 1;
            return UID(uid.fetch_add(1, std::memory_order_relaxed));
        }

    public:
        static const UID Invalid;

    private:
        uint64_t value = 0;
    };

    inline constexpr const UID UID::Invalid = UID(0);
}

namespace std 
{
    template <>
    struct hash<::Pitaya::Core::UID> 
    {
        size_t operator()(const ::Pitaya::Core::UID& uid) const noexcept
        {
            return std::hash<uint64_t>{}(uid.GetValue());
        }
    };
}
