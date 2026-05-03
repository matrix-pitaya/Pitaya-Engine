#pragma once

#include<Core/Layout/def.h>
#include<glm.hpp>

namespace Pitaya::Renderer
{
	struct STD140_LAYOUT CSMShadowInfo
	{
		inline static constexpr const uint8_t CSM_LEVELS = 4;

		glm::mat4 LightVP[CSM_LEVELS];      // 级联的灯光视投影矩阵
		glm::vec4 CascadeSplits;            // xyzw分别存储相机空间的分割深度
	};
}
