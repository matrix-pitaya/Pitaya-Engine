#pragma once

#include<Physics/Common/PhysicsCommandType.h>
#include<Physics/Common/PhysicsBody.h>
#include<Core/Container/SlotMap.h>
#include<glm.hpp>
#include<gtc/quaternion.hpp>

namespace Pitaya::Physics
{
    struct SetTransformCommand
    {
        inline static constexpr const PhysicsCommandType Type = PhysicsCommandType::SetTransform;

        Pitaya::Core::SlotMap<Pitaya::Physics::PhysicsBody>::Handle Handle;
        glm::vec3 Position = glm::vec3(0.0f);
        glm::quat Rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
    };
}
