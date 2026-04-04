#pragma once

#include<cstdint>

namespace Pitaya::Render
{
	enum class RenderLayer : uint32_t 
    {
        Empty         = 0,              //空标志位

        Default       = 1 << 0, 
        Transparent   = 1 << 1,
        UI            = 1 << 2,

#if PITAYA_EDITOR
        EditorOnly    = 1 << 31 
#endif
    };
}