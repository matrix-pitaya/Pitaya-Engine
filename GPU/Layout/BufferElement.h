#pragma once

#include<GPU/Frontend/Shader/Shader.h>

#include<string>

namespace Pitaya::GPU
{
    struct BufferElement 
    {
        BufferElement() = default;
        BufferElement(Pitaya::GPU::ShaderVariableType type, uint32_t Location, bool normalized = false, bool instanced = false)
            : Type(type), Location(Location), Size(Pitaya::GPU::GetShaderVariableTypeSize(type)), Offset(0), Normalized(normalized), Instanced(instanced){ }

        //获取向量维度
        inline uint32_t GetComponentCount() const noexcept
        {
            switch (Type) 
            {
                case Pitaya::GPU::ShaderVariableType::Float:   return 1;
                case Pitaya::GPU::ShaderVariableType::Float2:  return 2;
                case Pitaya::GPU::ShaderVariableType::Float3:  return 3;
                case Pitaya::GPU::ShaderVariableType::Float4:  return 4;
                case Pitaya::GPU::ShaderVariableType::Mat3:    return 3;
                case Pitaya::GPU::ShaderVariableType::Mat4:    return 4;
                case Pitaya::GPU::ShaderVariableType::Int:     return 1;
                case Pitaya::GPU::ShaderVariableType::Int2:    return 2;
                case Pitaya::GPU::ShaderVariableType::Int3:    return 3;
                case Pitaya::GPU::ShaderVariableType::Int4:    return 4;
                case Pitaya::GPU::ShaderVariableType::Bool:    return 1;
                case Pitaya::GPU::ShaderVariableType::Invalid: return 0;
                default:                                           return 0;
            }
        }

    public:
        uint32_t Size = 0;
        uint32_t Offset = 0;
        uint32_t Location = 0;
        bool Normalized = false;   
        bool Instanced = false;
        Pitaya::GPU::ShaderVariableType Type = Pitaya::GPU::ShaderVariableType::Invalid;
    };
}
