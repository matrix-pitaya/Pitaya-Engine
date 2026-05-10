#pragma once

#include<string>
#include<cstdint>

namespace Pitaya::Render
{
	enum class PostProcessType : uint32_t
	{
		Bilt = 0,
		GammaCorrection,
		Bloom,
		ToneMapping,

		Invalid
	};

	inline std::string_view ToString(PostProcessType type) noexcept
	{
		switch (type)
		{
			case PostProcessType::Bilt:				return "Bilt";
			case PostProcessType::GammaCorrection:	return "GammaCorrection";
			case PostProcessType::Bloom:			return "Bloom";
			case PostProcessType::ToneMapping:		return "ToneMapping";
			case PostProcessType::Invalid:			return "Invalid";
			default:								return "Unknown";
		}
	}
}