#pragma once

#include<Core/Layout/def.h>
#include<glm.hpp>

namespace Pitaya::Render
{
	struct STD140_LAYOUT SceneInfo
	{
		glm::vec4 AmbientColor;
		uint64_t EnvCubemapHandle;
		uint64_t IrradianceHandle;
		uint64_t PrefilteredHandle;
		uint64_t BRDFLUTHandle;
		float DeltaTime;
		uint32_t _pad0;
		uint32_t _pad1;
		uint32_t _pad2;
	};
	static_assert(sizeof(SceneInfo) == 64, "SceneInfo must be 64 bytes for std140");
}
