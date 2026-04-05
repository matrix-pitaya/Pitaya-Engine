#pragma once

#include<cstdint>
#include<Core/Utils/String.h>

namespace Pitaya::GPU
{
	enum class TextureCubemapFace : uint8_t
	{
		Px = 0,
		Py,
		Pz,
		Nx,
		Ny,
		Nz,

		Unknown
	};

	template<typename T>
	T ToEnum(std::string_view) noexcept;

	template<>
	inline Pitaya::GPU::TextureCubemapFace ToEnum<Pitaya::GPU::TextureCubemapFace>(std::string_view strview) noexcept
	{
		std::string str = Pitaya::Core::ToLower(strview);
		if (str == "right") { return Pitaya::GPU::TextureCubemapFace::Px; }
		else if (str == "left") { return Pitaya::GPU::TextureCubemapFace::Py; }
		else if (str == "top") { return Pitaya::GPU::TextureCubemapFace::Pz; }
		else if (str == "bottom") { return Pitaya::GPU::TextureCubemapFace::Nx; }
		else if (str == "front") { return Pitaya::GPU::TextureCubemapFace::Ny; }
		else if (str == "back") { return Pitaya::GPU::TextureCubemapFace::Nz; }
		else { return Pitaya::GPU::TextureCubemapFace::Unknown; }
	}
}
