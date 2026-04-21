#pragma once

#include<cstdint>
#include<string_view>

namespace Pitaya::Render
{
	enum class RenderLayer : uint32_t 
    {
        Empty         = 0,              //空标志位

        Default       = 1 << 0, 
        Transparent   = 1 << 1,
        UI            = 1 << 2,

        EditorOnly    = 1 << 31 
    };

    inline std::string_view ToString(RenderLayer layer) noexcept
    {
        switch (layer)
        {
            case RenderLayer::Empty:        return "Empty";
            case RenderLayer::Default:      return "Default";
            case RenderLayer::Transparent:  return "Transparent";
            case RenderLayer::UI:           return "UI";
            case RenderLayer::EditorOnly:   return "Editor Only";
            default: return "Unknown";
        }
	}
}
