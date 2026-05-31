#pragma once

#include<Physics/Common/PhysicsCommandType.h>
#include<Physics/Common/PhysicsBody.h>
#include<Core/Container/SlotMap.h>
#include<glm.hpp>

namespace Pitaya::Physics
{
    struct AddTorqueCommand
    {
        inline static constexpr const PhysicsCommandType Type = PhysicsCommandType::AddTorque;

        Pitaya::Core::SlotMap<Pitaya::Physics::PhysicsBody>::Handle Handle;
        glm::vec3 Torque = glm::vec3(0.0f);
    };
}
