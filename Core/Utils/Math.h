#pragma once

#include<Core/Math/Plane.h>

namespace Pitaya::Core
{
    //计算 3 个面的交点
    //公式：P = -(d1(n2 x n3) + d2(n3 x n1) + d3(n1 x n2)) / dot(n1, n2 x n3)
    inline glm::vec3 CalculatePlaneIntersection(const Plane& p1, const Plane& p2, const Plane& p3)
    {
        glm::vec3 n1 = p1.Normal;
        glm::vec3 n2 = p2.Normal;
        glm::vec3 n3 = p3.Normal;
        float det = glm::dot(n1, glm::cross(n2, n3));
        glm::vec3 v = -p1.Length * glm::cross(n2, n3)
            - p2.Length * glm::cross(n3, n1)
            - p3.Length * glm::cross(n1, n2);
        return v / det;
    }
}