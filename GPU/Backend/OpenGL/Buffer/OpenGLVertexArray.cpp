#include<GPU/Frontend/Buffer/VertexArray.h>
#include<Core/Allocate/Allocate.h>
#include<cstdint>
#include<vector>

#if defined(PITAYA_GRAPHICS_OPENGL)
#include<GL/glew.h>

namespace 
{
    inline GLenum ShaderVariableTypeToOpenGLBaseType(Pitaya::GPU::ShaderVariableType type) noexcept
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
    inline void BindFloatAttribute(Pitaya::GPU::BufferElement element, Pitaya::GPU::BufferLayout layout)
    {
        glEnableVertexAttribArray(element.Location);
        glVertexAttribPointer(
            element.Location,
            element.GetComponentCount(),
            ShaderVariableTypeToOpenGLBaseType(element.Type),
            element.Normalized ? GL_TRUE : GL_FALSE,
            layout.GetStride(),
            (const void*)(uintptr_t)element.Offset);
        glVertexAttribDivisor(element.Location, element.Instanced ? 1 : 0);
    }
    inline void BindIntAttribute(Pitaya::GPU::BufferElement element, Pitaya::GPU::BufferLayout layout)
    {
        glEnableVertexAttribArray(element.Location);
        glVertexAttribIPointer(
            element.Location,
            element.GetComponentCount(),
            ShaderVariableTypeToOpenGLBaseType(element.Type),
            layout.GetStride(),
            (const void*)(uintptr_t)element.Offset);
        glVertexAttribDivisor(element.Location, element.Instanced ? 1 : 0);
    }
    inline void BindMatrixAttribute(Pitaya::GPU::BufferElement element, Pitaya::GPU::BufferLayout layout)
    {
        uint8_t count = element.GetComponentCount();
        for (uint8_t i = 0; i < count; i++)
        {
            uint32_t currentLocation = element.Location + i;
            glEnableVertexAttribArray(currentLocation);
            glVertexAttribPointer(
                currentLocation,
                count,
                ShaderVariableTypeToOpenGLBaseType(element.Type),
                element.Normalized ? GL_TRUE : GL_FALSE,
                layout.GetStride(),
                (const void*)(uintptr_t)(element.Offset + sizeof(float) * count * i));
            glVertexAttribDivisor(currentLocation, element.Instanced ? 1 : 0);
        }
    }
}

Pitaya::GPU::VertexArray Pitaya::GPU::VertexArray::Factory::Create()
{
    GLuint VAO = 0;
    glGenVertexArrays(1, &VAO);
    return { VAO };
}
void Pitaya::GPU::VertexArray::Factory::Destroy(Pitaya::GPU::VertexArray vertexarray)
{
    GLuint VAO = vertexarray.Id;
    if (VAO > 0) { glDeleteVertexArrays(1, &VAO); }
}
void Pitaya::GPU::VertexArray::Operate::Link(Pitaya::GPU::VertexArray vertexArray, Pitaya::GPU::VertexBuffer vertexBuffer, Pitaya::GPU::IndexBuffer indexBuffer)
{
    GLuint VAO = vertexArray.Id;
    glBindVertexArray(VAO);
    GLuint VBO = vertexBuffer.Id;
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    const auto& layout = vertexBuffer.layout;
    for (const auto& element : layout.GetElements())
    {
        switch (element.Type)
        {
            case ShaderVariableType::Float:
            case ShaderVariableType::Float2:
            case ShaderVariableType::Float3:
            case ShaderVariableType::Float4:
                BindFloatAttribute(element, layout);
                break;

            case ShaderVariableType::Int:
            case ShaderVariableType::Int2:
            case ShaderVariableType::Int3:
            case ShaderVariableType::Int4:
            case ShaderVariableType::Bool:
                BindIntAttribute(element, layout);
                break;

            case ShaderVariableType::Mat3:
            case ShaderVariableType::Mat4:
                BindMatrixAttribute(element, layout);
                break;
        }
    }
    GLuint EBO = indexBuffer.Id;
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
#endif