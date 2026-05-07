#pragma once

#include<Core/Math/AABB.h>
#include<Core/Math/Plane.h>

namespace Pitaya::Core
{
    struct Frustum
    {
        Plane Planes[6] = {};

        Frustum() = default;
        Frustum(glm::mat4 vp)
        {
            Planes[0] = Plane({ vp[0][3] + vp[0][0], vp[1][3] + vp[1][0], vp[2][3] + vp[2][0], vp[3][3] + vp[3][0] }); // Left
            Planes[1] = Plane({ vp[0][3] - vp[0][0], vp[1][3] - vp[1][0], vp[2][3] - vp[2][0], vp[3][3] - vp[3][0] }); // Right
            Planes[2] = Plane({ vp[0][3] + vp[0][1], vp[1][3] + vp[1][1], vp[2][3] + vp[2][1], vp[3][3] + vp[3][1] }); // Bottom
            Planes[3] = Plane({ vp[0][3] - vp[0][1], vp[1][3] - vp[1][1], vp[2][3] - vp[2][1], vp[3][3] - vp[3][1] }); // Top
            Planes[4] = Plane({ vp[0][3] + vp[0][2], vp[1][3] + vp[1][2], vp[2][3] + vp[2][2], vp[3][3] + vp[3][2] }); // Near
            Planes[5] = Plane({ vp[0][3] - vp[0][2], vp[1][3] - vp[1][2], vp[2][3] - vp[2][2], vp[3][3] - vp[3][2] }); // Far
        }

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
