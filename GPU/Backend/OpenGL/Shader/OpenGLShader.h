#pragma once

#include<GPU/Frontend/Shader/Shader.h>

#include<GL/glew.h>
#include<GLFW/glfw3.h>

#include<exception>
#include<stdexcept>

namespace Pitaya::GPU
{
    class OpenGLShader : public Shader
    {
    private:
        struct ShaderSourceInfo
        {
            GLenum type;
            const char* source = nullptr;
        };
    public:
        OpenGLShader(const char* vertexSource, const char* fragmentSource)
            :Shader(vertexSource, fragmentSource)
        {
            Init({ { GL_VERTEX_SHADER, vertexSource },
                { GL_FRAGMENT_SHADER, fragmentSource } });
        }
        OpenGLShader(const char* vertexSource, const char* fragmentSource, const char* geometrySource)
            :Shader(vertexSource, fragmentSource, geometrySource)
        {
            Init({ { GL_VERTEX_SHADER, vertexSource },
                { GL_FRAGMENT_SHADER, fragmentSource },
                { GL_GEOMETRY_SHADER, geometrySource } });
        }
        ~OpenGLShader() override
        {
            if (programId)
            {
                glDeleteProgram(programId);
                programId = 0;
            }
        }

    public:
        Pitaya::GPU::Identifier<Shader> GetGPUIdentifier() const override
        {
            return programId;
        }

    private:
        void Init(const std::vector<ShaderSourceInfo>& shaders)
        {
            if (shaders.empty()) { throw std::runtime_error("No shaders provided!"); }

            programId = glCreateProgram(); 
            std::vector<GLuint> compiledShaders;
            for (const auto& info : shaders)
            {
                if (!info.source)
                {
                    CleanupShaders(compiledShaders);
                    glDeleteProgram(programId);
                    throw std::runtime_error("Shader source is empty!");
                }

                GLuint shaderId = glCreateShader(info.type);
                glShaderSource(shaderId, 1, &info.source, NULL);
                glCompileShader(shaderId);

                int success = 0;
                glGetShaderiv(shaderId, GL_COMPILE_STATUS, &success);
                if (!success)
                {
                    char buffer[1024] = {};
                    glGetShaderInfoLog(shaderId, sizeof(buffer), NULL, buffer);

                    std::string typeStr;
                    if (info.type == GL_VERTEX_SHADER) { typeStr = "Vertex"; }
                    else if (info.type == GL_FRAGMENT_SHADER) { typeStr = "Fragment"; }
                    else if (info.type == GL_GEOMETRY_SHADER) { typeStr = "Geometry"; }

                    glDeleteShader(shaderId);
                    CleanupShaders(compiledShaders);
                    glDeleteProgram(programId);
                    throw std::runtime_error(typeStr + " Compile Fail: " + buffer);
                }

                glAttachShader(programId, shaderId);
                compiledShaders.push_back(shaderId);
            }

            glLinkProgram(programId);
            int success = 0;
            glGetProgramiv(programId, GL_LINK_STATUS, &success);
            if (!success)
            {
                char buffer[1024] = {};
                glGetProgramInfoLog(programId, sizeof(buffer), NULL, buffer);

                CleanupShaders(compiledShaders);
                glDeleteProgram(programId);
                throw std::runtime_error("Shader Link Fail: " + std::string(buffer));
            }

            for (auto id : compiledShaders)
            {
                glDetachShader(programId, id);
                glDeleteShader(id);
            }
        }
        void CleanupShaders(const std::vector<GLuint>& shaderIds)
        {
            for (auto id : shaderIds)
            {
                glDeleteShader(id);
            }
        }

    public:
        static GLenum ShaderVariableTypeToOpenGLBaseType(Pitaya::GPU::ShaderVariableType type) noexcept
        {
            switch (type)
            {
                case Pitaya::GPU::ShaderVariableType::Float:    return GL_FLOAT;
                case Pitaya::GPU::ShaderVariableType::Float2:   return GL_FLOAT;
                case Pitaya::GPU::ShaderVariableType::Float3:   return GL_FLOAT;
                case Pitaya::GPU::ShaderVariableType::Float4:   return GL_FLOAT;
                case Pitaya::GPU::ShaderVariableType::Mat3:     return GL_FLOAT;
                case Pitaya::GPU::ShaderVariableType::Mat4:     return GL_FLOAT;
                case Pitaya::GPU::ShaderVariableType::Int:      return GL_INT;
                case Pitaya::GPU::ShaderVariableType::Int2:     return GL_INT;
                case Pitaya::GPU::ShaderVariableType::Int3:     return GL_INT;
                case Pitaya::GPU::ShaderVariableType::Int4:     return GL_INT;
                case Pitaya::GPU::ShaderVariableType::Bool:     return GL_BOOL;
                default: return 0;
            }
        }

    private:
        GLuint programId = 0;
    };
}
