#pragma once

#include<Renderer/Runtime/Shader/Shader.h>

#include<GL/glew.h>
#include<GLFW/glfw3.h>

namespace Pitaya::Engine::Renderer
{
	class OpenGLShader : public Shader
	{

    public:
        inline static GLenum ShaderDataTypeToOpenGLBaseType(ShaderDataType type) noexcept
        {
            switch (type)
            {
                case ShaderDataType::Float:    return GL_FLOAT;
                case ShaderDataType::Float2:   return GL_FLOAT;
                case ShaderDataType::Float3:   return GL_FLOAT;
                case ShaderDataType::Float4:   return GL_FLOAT;
                case ShaderDataType::Mat3:     return GL_FLOAT;
                case ShaderDataType::Mat4:     return GL_FLOAT;
                case ShaderDataType::Int:      return GL_INT;
                case ShaderDataType::Int2:     return GL_INT;
                case ShaderDataType::Int3:     return GL_INT;
                case ShaderDataType::Int4:     return GL_INT;
                case ShaderDataType::Bool:     return GL_BOOL;
                case ShaderDataType::Unknown:  return 0;
                default:                       return 0;
            }
        }
	};
}