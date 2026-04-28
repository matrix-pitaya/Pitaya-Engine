#pragma once

#include<cstdint>

namespace Pitaya::GPU
{
	enum class UBOBindPoint : uint32_t
	{
		CameraSnapshot = 0,
		PostProcessUBO = 1
	};

	enum class SSBOBindPoint : uint32_t
	{
		InstanceModelTransform = 0,
		BoneInverseMatrice = 1,
		SceneLights = 2
	};
}
