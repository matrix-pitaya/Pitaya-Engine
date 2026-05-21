#pragma once

#include<Core/Container/SlotMap.h>
#include<GPU/Common/GPUObjectType.h>

namespace Pitaya::Render
{
    struct BRDFLUTBakeInput
    {
        Pitaya::Core::SlotMap<Pitaya::GPU::Texture2D>::Handle Output;
        uint32_t Resolution = 128;
    };
}
