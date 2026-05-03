#pragma once

#include<cstddef>
#include<type_traits>

namespace Pitaya::Core
{
    template<size_t Size, size_t Align = alignof(std::max_align_t)>
    struct Storage
    {
        static_assert(Size > 0);
    public:
        template<typename T>
        inline T& Cast() 
        {
            static_assert(sizeof(T) <= Size, "Type size exceeds allocated space.");
            static_assert(Align % alignof(T) == 0, "Alignment requirement not met.");
            return *reinterpret_cast<T*>(data);
        }
        template<typename T>
        inline const T& Cast() const
        {
            static_assert(sizeof(T) <= Size, "Type size exceeds allocated space.");
            return *reinterpret_cast<const T*>(data);
        }

    private:
        alignas(Align) std::byte data[Size] = {};
    };
}
