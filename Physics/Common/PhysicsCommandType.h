#pragma once

#include<cstdint>

namespace Pitaya::Physics
{
    enum class PhysicsCommandType : uint32_t
    {
        CreateBody = 0,
        DestroyBody,
        SetTransform,
        SetLinearVelocity,
        SetAngularVelocity,
        AddForce,
        AddTorque,
        SetGravity,
        UpdateBodyParams,

        Invalid
    };

    inline constexpr const size_t PhysicsCommandTypeCount = static_cast<size_t>(PhysicsCommandType::Invalid);
}
