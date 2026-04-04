#pragma once

#include<glm.hpp>

namespace Pitaya::Core
{
    struct Plane
    {
        glm::vec3 Normal = glm::vec3(0.0f);
        float Length = 0.0f;

        inline float Distance(const glm::vec3& p) const
        {
            return glm::dot(Normal, p) + Length;
        }
    };
}
