#pragma once

#include<cstdint>

namespace Pitaya::Thread
{
    enum class ThreadType : uint32_t
    {
        Main = 0,
        Render,
        Log,
        Physics,
        Job,
        Invalid
    };

    inline constexpr const size_t PrimaryThreadTypeCount = static_cast<size_t>(ThreadType::Job);

    inline bool GetIsPrimaryThreadType(ThreadType type) noexcept
    {
        return static_cast<size_t>(type) < PrimaryThreadTypeCount;
	}
}
