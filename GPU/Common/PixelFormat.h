#pragma once

#include<cstdint>
#include<string_view>

namespace Pitaya::GPU
{
	enum class PixelFormat : uint32_t
	{
		// LDR Color (Linear 8-bit)
		R8 = 0,
		RG8,
		RGB8,
		RGBA8,

		// sRGB Color (硬件采样时自动 sRGB->Linear)
		SRGB8_A8,

		// HDR Color (Float)
		R16F,
		RG16F,
		RGB16F,
		RGBA16F,
		R32F,

		// Depth / Stencil
		Depth24_Stencil8,
		Depth32F,

		Invalid
	};

	inline uint32_t PixelFormatBytesPerPixel(PixelFormat format) noexcept
	{
		switch (format)
		{
			case Pitaya::GPU::PixelFormat::R8:					return 1;
			case Pitaya::GPU::PixelFormat::RG8:					return 2;
			case Pitaya::GPU::PixelFormat::RGB8:				return 3;
			case Pitaya::GPU::PixelFormat::RGBA8:				return 4;
			case Pitaya::GPU::PixelFormat::SRGB8_A8:			return 4;
			case Pitaya::GPU::PixelFormat::R16F:				return 2;
			case Pitaya::GPU::PixelFormat::RG16F:				return 4;
			case Pitaya::GPU::PixelFormat::RGB16F:				return 6;
			case Pitaya::GPU::PixelFormat::RGBA16F:				return 8;
			case Pitaya::GPU::PixelFormat::R32F:				return 4;
			case Pitaya::GPU::PixelFormat::Depth24_Stencil8:	return 4;
			case Pitaya::GPU::PixelFormat::Depth32F:			return 4;
			case Pitaya::GPU::PixelFormat::Invalid:				return 0;
			default:											return 0;
		}
	}
	inline bool PixelFormatIsSRGB(PixelFormat format) noexcept
	{
		return format == Pitaya::GPU::PixelFormat::SRGB8_A8;
	}
	inline bool PixelFormatIsDepth(PixelFormat format) noexcept
	{
		return format == Pitaya::GPU::PixelFormat::Depth24_Stencil8 || format == Pitaya::GPU::PixelFormat::Depth32F;
	}
	inline std::string_view ToString(PixelFormat format) noexcept
	{
		switch (format)
		{
			case Pitaya::GPU::PixelFormat::R8:					return "R8";
			case Pitaya::GPU::PixelFormat::RG8:					return "RG8";
			case Pitaya::GPU::PixelFormat::RGB8:				return "RGB8";
			case Pitaya::GPU::PixelFormat::RGBA8:				return "RGBA8";
			case Pitaya::GPU::PixelFormat::SRGB8_A8:			return "SRGB8_A8";
			case Pitaya::GPU::PixelFormat::R16F:				return "R16F";
			case Pitaya::GPU::PixelFormat::RG16F:				return "RG16F";
			case Pitaya::GPU::PixelFormat::RGB16F:				return "RGB16F";
			case Pitaya::GPU::PixelFormat::RGBA16F:				return "RGBA16F";
			case Pitaya::GPU::PixelFormat::R32F:				return "R32F";
			case Pitaya::GPU::PixelFormat::Depth24_Stencil8:	return "Depth24_Stencil8";
			case Pitaya::GPU::PixelFormat::Depth32F:			return "Depth32F";
			case Pitaya::GPU::PixelFormat::Invalid:				return "Invalid";
			default:											return "Invalid";
		}
	}

	template<typename T>
	T ToEnum(std::string_view) noexcept;

	template<>
	inline Pitaya::GPU::PixelFormat ToEnum<Pitaya::GPU::PixelFormat>(std::string_view str) noexcept
	{
		if (str == "R8") { return PixelFormat::R8; }
		else if (str == "RG8") { return PixelFormat::RG8; }
		else if (str == "RGB8") { return PixelFormat::RGB8; }
		else if (str == "RGBA8") { return PixelFormat::RGBA8; }
		else if (str == "SRGB8_A8") { return PixelFormat::SRGB8_A8; }
		else if (str == "R16F") { return PixelFormat::R16F; }
		else if (str == "RG16F") { return PixelFormat::RG16F; }
		else if (str == "RGB16F") { return PixelFormat::RGB16F; }
		else if (str == "RGBA16F") { return PixelFormat::RGBA16F; }
		else if (str == "R32F") { return PixelFormat::R32F; }
		else if (str == "Depth24_Stencil8") { return PixelFormat::Depth24_Stencil8; }
		else if (str == "Depth32F") { return PixelFormat::Depth32F; }
		else { return PixelFormat::Invalid; }
	}
}
