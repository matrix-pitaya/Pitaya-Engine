#pragma once

#include<Core/Container/SlotMap.h>
#include<GPU/Common/GPUObjectType.h>

namespace Pitaya::Render
{
    struct IBLBakeInput
    {
        Pitaya::Core::SlotMap<Pitaya::GPU::Texture2D>::Handle Equirect;
        Pitaya::Core::SlotMap<Pitaya::GPU::TextureCubemap>::Handle EnvCubemap;
        Pitaya::Core::SlotMap<Pitaya::GPU::TextureCubemap>::Handle Irradiance;
        Pitaya::Core::SlotMap<Pitaya::GPU::TextureCubemap>::Handle Prefiltered;
    };
}