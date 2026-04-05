#pragma once

#include<cstdint>
#include<Core/Utils/String.h>
#include<Core/Utils/StateFlags.h>

namespace Pitaya::GPU
{
	enum class ShaderType : uint32_t
	{
		Empty = 0,

		Vertex = 1 << 0,
		Geometry = 1 << 1,
		Fragment = 1 << 2,
	};

	template<typename T>
	T ToEnum(std::string_view) noexcept;

	template<>
	inline Pitaya::GPU::ShaderType ToEnum<Pitaya::GPU::ShaderType>(std::string_view strview) noexcept
	{
		std::string str = Pitaya::Core::ToLower(strview);
		if (str == "vertex") { return Pitaya::GPU::ShaderType::Vertex; }
		else if (str == "fragment") { return Pitaya::GPU::ShaderType::Fragment; }
		else if (str == "geometry") { return Pitaya::GPU::ShaderType::Geometry; }
		else { return Pitaya::GPU::ShaderType::Empty; }
	}
}
