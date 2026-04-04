#pragma once

#include<cstdint>

namespace Pitaya::GPU
{
	enum class MeshType : uint8_t
	{
		Static = 0,
		SkinnedMesh,

		Unknwon
	};
}
