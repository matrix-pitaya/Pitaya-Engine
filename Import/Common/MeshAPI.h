#pragma once

#include<cstdint>
#include<string_view>

namespace Pitaya::Import
{
	enum class MeshAPI : uint8_t
	{
		Assimp = 0,

		Invalid
	};

	inline constexpr const size_t MeshAPICount = static_cast<size_t>(MeshAPI::Invalid);
	inline constexpr std::string_view ToString(MeshAPI api) noexcept
	{
		switch (api)
		{
			case MeshAPI::Assimp:	return "Assimp";
			case MeshAPI::Invalid:	return "Invalid";
			default:				return "Invalid";
		}
	}
}
