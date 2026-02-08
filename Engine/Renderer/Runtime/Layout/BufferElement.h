#pragma once

#include<Renderer/Runtime/Shader/Shader.h>

#include<string>

namespace Pitaya::Engine::Renderer
{
    struct BufferElement 
    {
        BufferElement() = default;
        BufferElement(ShaderDataType type, const std::string& name, bool normalized = false, bool instanced = false)
            : Name(name), Type(type), Size(Pitaya::Engine::Renderer::Shader::GetShaderDataTypeSize(type)), Offset(0), Normalized(normalized), Instanced(instanced){ }

        //获取向量维度
        inline uint32_t GetComponentCount() const noexcept
        {
            switch (Type) 
            {
                case ShaderDataType::Float:   return 1;
                case ShaderDataType::Float2:  return 2;
                case ShaderDataType::Float3:  return 3;
                case ShaderDataType::Float4:  return 4;
                case ShaderDataType::Mat3:    return 3;
                case ShaderDataType::Mat4:    return 4;
                case ShaderDataType::Int:     return 1;
                case ShaderDataType::Int2:    return 2;
                case ShaderDataType::Int3:    return 3;
                case ShaderDataType::Int4:    return 4;
                case ShaderDataType::Bool:    return 1;
                case ShaderDataType::Unknown: return 0;
                default:                      return 0;
            }
        }

    public:
        std::string Name;
        ShaderDataType Type = ShaderDataType::Unknown;
        uint32_t Size = 0;
        uint32_t Offset = 0;
        bool Normalized = false;   
        bool Instanced = false;
    };
}