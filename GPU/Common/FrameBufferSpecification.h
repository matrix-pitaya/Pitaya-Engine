#pragma once

#include<GPU/Common/PixelFormat.h>
#include<cstdint>

namespace Pitaya::GPU
{
    struct FrameBufferSpecification
    {
        uint32_t Width = 1600;
        uint32_t Height = 900;
        uint32_t Samples = 1;
        PixelFormat ColorFormat = PixelFormat::RGBA8;
        PixelFormat DepthFormat = PixelFormat::Depth24_Stencil8;   // Invalid 表示无深度附件
    };
}
