#pragma once

#include<cstdint>

namespace Pitaya::GPU
{
    struct FrameBufferSpecification
    {
        uint32_t Width = 0;
        uint32_t Height = 0;
        uint32_t Samples = 1;
        bool SwapChainTarget = false;
        bool HDR = false;     
        bool HasDepth = true; 
    };
}
