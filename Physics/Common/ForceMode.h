#pragma once

#include<cstdint>

namespace Pitaya::Physics
{
    enum class ForceMode : uint8_t
    {
        Force,
        Impulse,
        VelocityChange,
        Acceleration,
        Invalid
    };
}
