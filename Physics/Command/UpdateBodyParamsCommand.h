#pragma once

#include<Physics/Common/PhysicsCommandType.h>
#include<Physics/Common/PhysicsBody.h>
#include<Core/Container/SlotMap.h>

namespace Pitaya::Physics
{
    struct UpdateBodyParamsCommand
    {
        inline static constexpr const PhysicsCommandType Type = PhysicsCommandType::UpdateBodyParams;

        Pitaya::Core::SlotMap<Pitaya::Physics::PhysicsBody>::Handle Handle;
        float Mass = 1.0f;
        float LinearDamping = 0.0f;
        float AngularDamping = 0.0f;
        float Restitution = 0.0f;
        float Friction = 0.5f;
    };
}
