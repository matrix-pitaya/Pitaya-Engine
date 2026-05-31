#pragma once

#include<Physics/Common/PhysicsCommandType.h>
#include<glm.hpp>

namespace Pitaya::Physics
{
    struct SetGravityCommand
    {
        inline static constexpr const PhysicsCommandType Type = PhysicsCommandType::SetGravity;

        glm::vec3 Gravity = glm::vec3(0.0f, -9.8f, 0.0f);
    };
}
