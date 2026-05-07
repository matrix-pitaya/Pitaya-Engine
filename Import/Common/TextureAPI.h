#pragma once

#include<cstdint>
#include<string_view>

namespace Pitaya::Import
{
	enum class TextureAPI : uint8_t
	{
		StbImage = 0,

		Invalid
	};

	inline constexpr const size_t TextureAPICount = static_cast<size_t>(TextureAPI::Invalid);
	inline constexpr std::string_view ToString(TextureAPI api) noexcept
	{
		switch (api)
		{
			case TextureAPI::StbImage:	return "StbImage";
			case TextureAPI::Invalid:	return "Invalid";
			default:					return "Invalid";
		}
	}
}
