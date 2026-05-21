#pragma once

#include<Core/Container/SlotMap.h>
#include<GPU/Frontend/Texture/Texture2D.h>
#include<GPU/Frontend/Texture/TextureCubemap.h>

namespace Pitaya::Asset
{
	struct SkyBox
	{
		Pitaya::Core::SlotMap<Pitaya::GPU::Texture2D>::Handle EquirectHandle;
		Pitaya::Core::SlotMap<Pitaya::GPU::TextureCubemap>::Handle EnvCubemapHandle;
		Pitaya::Core::SlotMap<Pitaya::GPU::TextureCubemap>::Handle IrradianceHandle;
		Pitaya::Core::SlotMap<Pitaya::GPU::TextureCubemap>::Handle PrefilteredHandle;
	};
}
