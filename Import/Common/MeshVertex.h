#pragma once

#include<glm.hpp>

namespace Pitaya::Import
{
    // 静态网格顶点结构 (12 floats = 48 bytes)
    // 布局固定：Pos(3) + Norm(3) + UV(2) + Tan(4)
    struct StaticMeshVertex
    {
        glm::vec3 Position; // location: 0
        glm::vec3 Normal;   // location: 1
        glm::vec2 UV;       // location: 2
        glm::vec4 Tangent;  // location: 3 (w: handedness)
    };

    // 蒙皮网格顶点 (20 floats = 80 bytes)
    // 布局固定：Pos(3) + Norm(3) + UV(2) + Tan(4) + Weights(4) + Indices(4)
    struct SkinnedMeshVertex
    {
        glm::vec3 Position;     // location: 0
        glm::vec3 Normal;       // location: 1
        glm::vec2 UV;           // location: 2
        glm::vec4 Tangent;      // location: 3 (w: handedness)
        glm::vec4 BoneWeights;  // location: 4
        glm::vec4 BoneIndices;  // location: 5 (存储为 float 格式的 ID)
    };

}