#pragma once

#include<glm.hpp>

namespace Pitaya::Core
{
    struct Plane
    {
        glm::vec3 Normal = glm::vec3(0.0f);
        float Length = 0.0f;

        Plane() = default;
        Plane(glm::vec4 p)
        {
            float mag = glm::length(glm::vec3(p));
            Normal = glm::vec3(p) / mag;
            Length = p.w / mag;
        }
        
        inline float Distance(const glm::vec3& p) const
        {
            return glm::dot(Normal, p) + Length;
        }
        inline void Normalize()
        {
            float mag = glm::length(Normal);
            Normal /= mag;
            Length /= mag;
        }
    };
}
