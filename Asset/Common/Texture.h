#pragma once

#include<Core/Identifier/GUID.h>
#include<GPU/Common/Identifier.h>
#include<GPU/Common/TextureType.h>

namespace Pitaya::Asset
{
	struct Texture
	{
		Pitaya::GPU::Identifier<GPU::Texture> Id;
		Pitaya::GPU::TextureType Type = GPU::TextureType::Texture2D;

		//Texture 1-12
		inline static constexpr const Pitaya::Core::GUID White = Pitaya::Core::GUID("00000000-0000-0000-0000-000000000001");
	};
}
