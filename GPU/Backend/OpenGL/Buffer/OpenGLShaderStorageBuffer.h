#pragma once

#include<GPU/Frontend/Buffer/ShaderStorageBuffer.h>

#include<GL/glew.h>
#include<GLFW/glfw3.h>

namespace Pitaya::GPU
{
    class OpenGLShaderStorageBuffer : public ShaderStorageBuffer
    {
    public:
        OpenGLShaderStorageBuffer(uint32_t size, uint32_t bindingPoint)
            : ShaderStorageBuffer(size, bindingPoint)
        {
            glGenBuffers(1, &SSBO);
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, SSBO);
            glBufferData(GL_SHADER_STORAGE_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
            glBindBufferBase(GL_SHADER_STORAGE_BUFFER, bindingPoint, SSBO);
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
        }
        ~OpenGLShaderStorageBuffer() override
        {
            glDeleteBuffers(1, &SSBO);
            SSBO = 0;
        }

    public:
        void Bind(uint32_t point) const override
        {
            glBindBufferBase(GL_SHADER_STORAGE_BUFFER, point, SSBO);
        }
        void Unbind() const override
        {
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
        }

        void SetData(const void* data, uint32_t size, uint32_t offset = 0) const override
        {
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, SSBO);
            glBufferSubData(GL_SHADER_STORAGE_BUFFER, offset, size, data);
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
        }
        Identifier<ShaderStorageBuffer> GetGPUIdentifier() const override
        {
            return SSBO;
        }

    private:
        GLuint SSBO = 0;
    };
}
