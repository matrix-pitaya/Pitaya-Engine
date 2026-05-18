#pragma once

#include<cstdint>

namespace Pitaya::GPU
{
	enum class ShaderVariableType : uint32_t
	{
		Float = 0,
		Float2,
		Float3,
		Float4,
		Mat3,
		Mat4,
		Int,
		Int2,
		Int3,
		Int4,
		Bool,

		Invalid
	};

    inline uint32_t GetShaderVariableTypeSize(ShaderVariableType type) noexcept
    {
        switch (type)
        {
            case Pitaya::GPU::ShaderVariableType::Float:    return 4;
            case Pitaya::GPU::ShaderVariableType::Float2:   return 4 * 2;
            case Pitaya::GPU::ShaderVariableType::Float3:   return 4 * 3;
            case Pitaya::GPU::ShaderVariableType::Float4:   return 4 * 4;
            case Pitaya::GPU::ShaderVariableType::Mat3:     return 4 * 3 * 3;
            case Pitaya::GPU::ShaderVariableType::Mat4:     return 4 * 4 * 4;
            case Pitaya::GPU::ShaderVariableType::Int:      return 4;
            case Pitaya::GPU::ShaderVariableType::Int2:     return 4 * 2;
            case Pitaya::GPU::ShaderVariableType::Int3:     return 4 * 3;
            case Pitaya::GPU::ShaderVariableType::Int4:     return 4 * 4;
            case Pitaya::GPU::ShaderVariableType::Bool:     return 1;
            case Pitaya::GPU::ShaderVariableType::Invalid:  return 0;
            default:										return 0;
        }
    }
}
