#pragma once

#include<GPU/Common/PixelFormat.h>
#include<cstdint>

namespace Pitaya::Asset
{
	// 纹理用途 决定加载到 GPU 时的 PixelFormat 与颜色空间
	enum class TextureUsage : uint32_t
	{
		Color,		// sRGB 8-bit  → SRGB8_A8       (Albedo / Emission / UI 等颜色类)
		Linear,		// Linear 8-bit → RGBA8         (Metallic / Roughness / AO / Normal / Mask 等数据类)
		HDR,		// 16-bit float → RGBA16F       (HDR equirect / LUT)

		Invalid
	};

	inline Pitaya::GPU::PixelFormat TextureUsageToPixelFormat(TextureUsage usage) noexcept
	{
		switch (usage)
		{
			case TextureUsage::Color:	return Pitaya::GPU::PixelFormat::SRGB8_A8;
			case TextureUsage::Linear:	return Pitaya::GPU::PixelFormat::RGBA8;
			case TextureUsage::HDR:		return Pitaya::GPU::PixelFormat::RGBA16F;
			case TextureUsage::Invalid:	return Pitaya::GPU::PixelFormat::Invalid;
			default:					return Pitaya::GPU::PixelFormat::Invalid;
		}
	}
}
