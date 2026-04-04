#pragma once

#include<string>
#include<cstdint>

namespace Pitaya::Render
{
	enum class PostProcessType : uint8_t
	{
		GammaCorrection = 0,
		Bloom,
		ToneMapping,

		Unknown
	};

	inline std::string_view ToString(PostProcessType type) noexcept
	{
		switch (type)
		{
			case PostProcessType::GammaCorrection:	return "GammaCorrection";
			case PostProcessType::Bloom:			return "Bloom";
			case PostProcessType::ToneMapping:		return "ToneMapping";
			case PostProcessType::Unknown:			return "Unknown";
			default:								return "Unknown";
		}
	}
}