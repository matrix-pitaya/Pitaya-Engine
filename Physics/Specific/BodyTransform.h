#pragma once

#include<Physics/Specific/PhysicsBody.h>
#include<Core/Container/SlotMap.h>
#include<glm.hpp>
#include<gtc/quaternion.hpp>

namespace Pitaya::Physics
{
    struct BodyTransform
    {
        Pitaya::Core::SlotMap<Pitaya::Physics::PhysicsBody>::Handle Handle;
        glm::vec3 Position = glm::vec3(0.0f);
        glm::quat Rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
        glm::vec3 LinearVelocity = glm::vec3(0.0f);
        glm::vec3 AngularVelocity = glm::vec3(0.0f);
    };
}
