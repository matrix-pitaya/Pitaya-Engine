#pragma once

#include<Render/Common/ShadowInfo.h>
#include<vector>

namespace Pitaya::Render
{
	struct LightShadowSetup
	{
		const std::vector<ShadowCasterSlice>& Slices;
		const std::vector<glm::mat4>& Matrices;
		const std::vector<CascadeSplitInfo>& CascadeSplits;
		uint32_t DirCount = 0;
		uint32_t SpotCount = 0;
		uint32_t PointCount = 0;
	};
}
