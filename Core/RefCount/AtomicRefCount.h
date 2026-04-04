#pragma once

#include<cstdint>
#include<atomic>

namespace Pitaya::Core
{
	struct AtomicRefCount
	{
        inline void AddRefCount() noexcept
        {
            count.fetch_add(1, std::memory_order_relaxed);
        }
        inline bool ReduceRefCount() noexcept
        {
            uint32_t old = count.load(std::memory_order_relaxed);
            while (old > 0 && !count.compare_exchange_weak(old, old - 1,
                std::memory_order_release, std::memory_order_relaxed));
            return old <= 1;
        }
        inline bool HasRef() const noexcept
        {
            return count.load(std::memory_order_relaxed) > 0;
        }
        inline uint32_t GetRefCount() const noexcept
        { 
            return count.load(std::memory_order_relaxed);
        }

    private:
        std::atomic<uint32_t> count = 0;
	};
}
