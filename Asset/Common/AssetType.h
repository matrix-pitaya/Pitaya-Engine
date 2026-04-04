#pragma once

#include<cstdint>

namespace Pitaya::Asset
{
	enum class AssetType : uint8_t
	{
		Texture = 0,
		Shader,
		Material,
		Mesh,
		RenderTarget,

		Unknown
	};
}

