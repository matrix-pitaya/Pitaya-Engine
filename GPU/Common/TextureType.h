#pragma once

#include<Core/Utils/String.h>
#include<cstdint>

namespace Pitaya::GPU
{
	enum class TextureType : uint32_t
	{
		Texture2D = 0,
		Texture3D,
		TextureCubemap,
		Texture2DArray,

		Unknown
	};

	inline std::string_view ToString(Pitaya::GPU::TextureType type)
	{
		switch (type)
		{
			case Pitaya::GPU::TextureType::Texture2D:		return "Texture2D";
			case Pitaya::GPU::TextureType::Texture2DArray:	return "Texture2DArray";
			case Pitaya::GPU::TextureType::Texture3D:		return "Texture3D";
			case Pitaya::GPU::TextureType::TextureCubemap:	return "TextureCubemap";
			case Pitaya::GPU::TextureType::Unknown:			return "Unknown";
			default:										return "Unknown";
		}
	}
}
