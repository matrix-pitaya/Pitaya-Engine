#pragma once

#include<Physics/Specific/BodyTransform.h>
#include<vector>
#include<cstdint>

namespace Pitaya::Physics
{
    struct PhysicsSnapshot
    {
        std::vector<BodyTransform> Prev;
        std::vector<BodyTransform> Curr;
        double StepCompletionTime = 0.0;
        uint32_t StepSequence = 0;
    };
}
