#pragma once

#include<Core/Math/Frustum.h>

#include<glm.hpp>

namespace Pitaya::Core
{
	//绑定点0
	struct alignas(16) CameraSnapshot
	{
		glm::mat4 View;
		glm::mat4 Projection;
		glm::mat4 ViewProjection;
		glm::vec4 Position;	//保证std140内存布局

		inline Pitaya::Core::Frustum CreateFrustum() const noexcept
		{
            Pitaya::Core::Frustum frustum;
            const glm::mat4& vp = ViewProjection;

            // Left
            frustum.Planes[0].Normal.x = vp[0][3] + vp[0][0];
            frustum.Planes[0].Normal.y = vp[1][3] + vp[1][0];
            frustum.Planes[0].Normal.z = vp[2][3] + vp[2][0];
            frustum.Planes[0].Length = vp[3][3] + vp[3][0];

            // Right
            frustum.Planes[1].Normal.x = vp[0][3] - vp[0][0];
            frustum.Planes[1].Normal.y = vp[1][3] - vp[1][0];
            frustum.Planes[1].Normal.z = vp[2][3] - vp[2][0];
            frustum.Planes[1].Length = vp[3][3] - vp[3][0];

            // Bottom
            frustum.Planes[2].Normal.x = vp[0][3] + vp[0][1];
            frustum.Planes[2].Normal.y = vp[1][3] + vp[1][1];
            frustum.Planes[2].Normal.z = vp[2][3] + vp[2][1];
            frustum.Planes[2].Length = vp[3][3] + vp[3][1];

            // Top
            frustum.Planes[3].Normal.x = vp[0][3] - vp[0][1];
            frustum.Planes[3].Normal.y = vp[1][3] - vp[1][1];
            frustum.Planes[3].Normal.z = vp[2][3] - vp[2][1];
            frustum.Planes[3].Length = vp[3][3] - vp[3][1];

            // Near
            frustum.Planes[4].Normal.x = vp[0][3] + vp[0][2];
            frustum.Planes[4].Normal.y = vp[1][3] + vp[1][2];
            frustum.Planes[4].Normal.z = vp[2][3] + vp[2][2];
            frustum.Planes[4].Length = vp[3][3] + vp[3][2];

            // Far  
            frustum.Planes[5].Normal.x = vp[0][3] - vp[0][2];
            frustum.Planes[5].Normal.y = vp[1][3] - vp[1][2];
            frustum.Planes[5].Normal.z = vp[2][3] - vp[2][2];
            frustum.Planes[5].Length = vp[3][3] - vp[3][2];

            for (auto& plane : frustum.Planes)
            {
                float len = glm::length(plane.Normal);
                plane.Normal /= len;
                plane.Length /= len;
            }
            return frustum;
		}
	};
}
