#pragma once

#include<Core/Math/AABB.h>
#include<Core/Math/Plane.h>

namespace Pitaya::Core
{
    struct Frustum
    {
        Plane Planes[6] = {};

        inline bool IsVisible(AABB aabb) const noexcept
        {
            for (const Plane& plane : Planes)
            {
                glm::vec3 farPoint;
                farPoint.x = (plane.Normal.x > 0) ? aabb.Max.x : aabb.Min.x;
                farPoint.y = (plane.Normal.y > 0) ? aabb.Max.y : aabb.Min.y;
                farPoint.z = (plane.Normal.z > 0) ? aabb.Max.z : aabb.Min.z;
                
                if (plane.Distance(farPoint) < 0) 
                { 
                    return false;
                }
            }
            return true;
        }
    };
}
