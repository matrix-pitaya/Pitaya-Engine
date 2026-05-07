#pragma once

#include<Core/Container/SlotMap.h>
#include<Render/Common/RenderCommandType.h>
#include<GPU/Common/GPUObjectType.h>

namespace Pitaya::Render
{
    struct DrawSkyboxCommand
    {
        inline static constexpr const RenderCommandType Type = RenderCommandType::DrawSkybox;

        Pitaya::Core::SlotMap<Pitaya::GPU::TextureCubemap>::Handle CubemapHandle;
    };
}