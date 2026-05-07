#include<GPU/Frontend/Buffer/ShaderStorageBuffer.h>

#if defined(PITAYA_GRAPHICS_OPENGL)
#include<GL/glew.h>

Pitaya::GPU::ShaderStorageBuffer Pitaya::GPU::ShaderStorageBuffer::Factory::Create(uint32_t size, uint32_t bindingPoint)
{
    GLuint SSBO = 0;
    glGenBuffers(1, &SSBO);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, SSBO);
    glBufferData(GL_SHADER_STORAGE_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, bindingPoint, SSBO);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    return { SSBO, size, bindingPoint };
}
void Pitaya::GPU::ShaderStorageBuffer::Factory::Destroy(Pitaya::GPU::ShaderStorageBuffer shaderStorageBuffer)
{
    GLuint SSBO = shaderStorageBuffer.Id;
    if(SSBO > 0) { glDeleteBuffers(1, &SSBO); }
}
#endif