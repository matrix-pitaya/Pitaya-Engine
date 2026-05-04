#pragma once

#include<Core/Identifier/GUID.h>
#include<Core/Container/SlotMap.h>
#include<GPU/Common/Identifier.h>
#include<GPU/Common/TextureType.h>

namespace Pitaya::Asset
{
	struct Texture
	{
		union
		{
			Pitaya::Core::SlotMap<Pitaya::GPU::Texture2D>::Handle Texture2DHandle = {};
			Pitaya::Core::SlotMap<Pitaya::GPU::Texture2DArray>::Handle Texture2DArrayHandle;
			Pitaya::Core::SlotMap<Pitaya::GPU::TextureCubemap>::Handle TextureCubemapHandle;
		};
		Pitaya::GPU::TextureType Type = GPU::TextureType::Texture2D;

		//Texture 1-12
		inline static constexpr const Pitaya::Core::GUID White = Pitaya::Core::GUID("00000000-0000-0000-0000-000000000001");
	};
}
