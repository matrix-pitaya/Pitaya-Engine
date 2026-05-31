#pragma once

#include<Physics/Common/PhysicsCommandType.h>
#include<Physics/Common/PhysicsBody.h>
#include<Physics/Common/ForceMode.h>
#include<Core/Container/SlotMap.h>
#include<glm.hpp>

namespace Pitaya::Physics
{
    struct AddForceCommand
    {
        inline static constexpr const PhysicsCommandType Type = PhysicsCommandType::AddForce;

        Pitaya::Core::SlotMap<Pitaya::Physics::PhysicsBody>::Handle Handle;
        glm::vec3 Force = glm::vec3(0.0f);
        ForceMode Mode = ForceMode::Force;
    };
}
