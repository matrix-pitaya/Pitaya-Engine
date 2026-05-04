#include<GPU/Frontend/Shader/Shader.h>
#include<exception>
#include<stdexcept>

#if defined(PITAYA_USE_OPENGL)
#include<GL/glew.h>
#include<GLFW/glfw3.h>

namespace
{
    struct ShaderSourceInfo
    {
        GLenum type;
        const char* source = nullptr;
    };
    void CleanupOpenGLShaders(const std::vector<GLuint>& shaderIds)
    {
        for (auto id : shaderIds)
        {
            glDeleteShader(id);
        }
    }
    GLuint CreateOpenGLShaders(const std::vector<ShaderSourceInfo>& shaders)
    {
        if (shaders.empty()) { throw std::runtime_error("No shaders provided!"); }

        GLuint programId = glCreateProgram();
        std::vector<GLuint> compiledShaders;
        for (const auto& info : shaders)
        {
            if (!info.source)
            {
                CleanupOpenGLShaders(compiledShaders);
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
                CleanupOpenGLShaders(compiledShaders);
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

            CleanupOpenGLShaders(compiledShaders);
            glDeleteProgram(programId);
            throw std::runtime_error("Shader Link Fail: " + std::string(buffer));
        }

        for (auto id : compiledShaders)
        {
            glDetachShader(programId, id);
            glDeleteShader(id);
        }

        return programId;
    }
}

Pitaya::GPU::Shader Pitaya::GPU::Shader::Factory::Create(const char* vertexSource, const char* fragmentSource)
{
    return { CreateOpenGLShaders({{ GL_VERTEX_SHADER, vertexSource },
                { GL_FRAGMENT_SHADER, fragmentSource } }) };
}
Pitaya::GPU::Shader Pitaya::GPU::Shader::Factory::Create(const char* vertexSource, const char* fragmentSource, const char* geometrySource)
{
    return { CreateOpenGLShaders({ { GL_VERTEX_SHADER, vertexSource },
                { GL_FRAGMENT_SHADER, fragmentSource },
                { GL_GEOMETRY_SHADER, geometrySource } }) };
}
void Pitaya::GPU::Shader::Factory::Destroy(Pitaya::GPU::Shader shader)
{
    GLuint programId = shader.Id;
    if (programId)
    {
        glDeleteProgram(programId);
    }
}
#endif