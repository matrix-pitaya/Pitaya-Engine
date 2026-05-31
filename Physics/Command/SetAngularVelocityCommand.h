#pragma once

#include<Physics/Common/PhysicsCommandType.h>
#include<Physics/Common/PhysicsBody.h>
#include<Core/Container/SlotMap.h>
#include<glm.hpp>

namespace Pitaya::Physics
{
    struct SetAngularVelocityCommand
    {
        inline static constexpr const PhysicsCommandType Type = PhysicsCommandType::SetAngularVelocity;

        Pitaya::Core::SlotMap<Pitaya::Physics::PhysicsBody>::Handle Handle;
        glm::vec3 Velocity = glm::vec3(0.0f);
    };
}
