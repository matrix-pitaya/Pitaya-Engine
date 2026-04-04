#pragma once

#include<cstdint>

namespace Pitaya::Core
{
    struct RefCount
    {
        inline void AddRefCount() noexcept
        {
            count++;
        }
        inline bool ReduceRefCount() noexcept
        {
            return count ? --count == 0 : true;
        }
        inline bool HasRef() const noexcept
        {
            return count > 0;
        }
        inline uint32_t GetRefCount() const noexcept
        {
            return count;
        }

    private:
        uint32_t count = 0;
    };
}
