#pragma once

#include<Physics/Common/PhysicsCommandType.h>
#include<Physics/Common/PhysicsBody.h>
#include<Core/Container/SlotMap.h>

namespace Pitaya::Physics
{
    struct DestroyBodyCommand
    {
        inline static constexpr const PhysicsCommandType Type = PhysicsCommandType::DestroyBody;

        Pitaya::Core::SlotMap<Pitaya::Physics::PhysicsBody>::Handle Handle;
    };
}
