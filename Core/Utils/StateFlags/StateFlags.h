#pragma once

#include<type_traits>
#include<utility>
#include<cstdint>
#include<initializer_list>

namespace Pitaya::Core
{
    template <typename TEnum>
    inline std::underlying_type_t<TEnum> CombineMask(std::initializer_list<TEnum> flags) noexcept
    {
        static_assert(std::is_enum_v<TEnum>, "TEnum must be an enum type!");
        static_assert(std::is_same_v<std::underlying_type_t<TEnum>, uint8_t> || std::is_same_v<std::underlying_type_t<TEnum>, uint16_t> ||
            std::is_same_v<std::underlying_type_t<TEnum>, uint32_t> || std::is_same_v<std::underlying_type_t<TEnum>, uint64_t>,
            "Only support uint8_t/uint16_t/uint32_t/uint64_t as underlying type!");

        std::underlying_type_t<TEnum> state = 0;
        for (TEnum flag : flags) 
        { 
            state |= static_cast<std::underlying_type_t<TEnum>>(flag);
        }
        return state;
    }

    template <typename TEnum, typename... Args>
    inline std::underlying_type_t<TEnum> CombineMask(TEnum first, Args&&... rest) noexcept
    {
        return CombineMask({ first, std::forward<Args>(rest)... });
    }
}
