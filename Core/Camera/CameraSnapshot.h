#pragma once

#include<Core/Layout/def.h>
#include<Core/Math/Frustum.h>

#include<glm.hpp>

namespace Pitaya::Core
{
	//GPU
	struct STD140_LAYOUT CameraSnapshot
	{
		glm::mat4 View;
		glm::mat4 Projection;
		glm::mat4 ViewProjection;
		glm::vec4 Position;	//保证std140内存布局 w 表示 camera 索引

		inline Pitaya::Core::Frustum CreateFrustum() const noexcept
		{
            return { ViewProjection };
		}
	};
}
