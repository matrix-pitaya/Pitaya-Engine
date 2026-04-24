#pragma once

#include<Render/Common/RenderCommandType.h>

#include<glm.hpp>

namespace Pitaya::Render
{
    struct BlitToScreenCommand
    {
        inline static constexpr const RenderCommandType Type = RenderCommandType::BlitToScreen;

        glm::uvec2 Size;
    };
}