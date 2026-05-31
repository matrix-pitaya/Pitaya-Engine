#pragma once

#include<Physics/Common/PhysicsCommandType.h>
#include<Physics/Common/RigidBodyType.h>
#include<glm.hpp>
#include<entt/entt.hpp>

namespace Pitaya::Physics
{
    struct CreateBodyCommand
    {
        inline static constexpr const PhysicsCommandType Type = PhysicsCommandType::CreateBody;

        entt::entity EntityId = entt::null;
        RigidBodyType BodyType = RigidBodyType::Static;
        glm::mat4 InitialTransform = glm::mat4(1.0f);

        float Mass = 1.0f;
        float LinearDamping = 0.0f;
        float AngularDamping = 0.0f;
        float Restitution = 0.0f;
        float Friction = 0.5f;
        glm::bvec3 LockPosition = glm::bvec3(false);
        glm::bvec3 LockRotation = glm::bvec3(false);
        bool UseGravity = true;
        uint32_t CollisionLayer = 1;
        uint32_t CollisionMask = 0xFFFFFFFF;

        glm::vec3 InitialLinearVelocity = glm::vec3(0.0f);
        glm::vec3 InitialAngularVelocity = glm::vec3(0.0f);
    };
}
