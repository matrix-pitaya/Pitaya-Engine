#pragma once

#include<Renderer/Runtime/Vertex/VertexArray.h>
#include<Renderer/Backend/OpenGL/Shader/OpenGLShader.h>

#include<Utility/Utility.h>

#include<GL/glew.h>
#include<GLFW/glfw3.h>

#include<cstdint>

namespace Pitaya::Engine::Renderer
{
	class OpenGLVertexArray : public VertexArray
	{
	public:
        OpenGLVertexArray() 
        {
            glGenVertexArrays(1, &VAO);
        }
        ~OpenGLVertexArray() override
        {
            glDeleteVertexArrays(1, &VAO);
            VAO = 0;
            attributeIndex = 0;
            vertexBuffers.clear();
            elementBuffer = nullptr;
        }

        void Bind() const override 
        {
            glBindVertexArray(VAO);
        }
        void Unbind() const override
        {
            glBindVertexArray(0);
        }

        void AddVertexBuffer(VertexBuffer* vertexBuffer) override 
        {
            glBindVertexArray(VAO);
            vertexBuffer->Bind();

            const auto& layout = vertexBuffer->GetLayout();
            for (const auto& element : layout.GetElements())
            {
                switch (element.Type)
                {
                    case ShaderDataType::Float:
                    case ShaderDataType::Float2:
                    case ShaderDataType::Float3:
                    case ShaderDataType::Float4:
                        BindFloatAttribute(element, layout);
                        break;

                    case ShaderDataType::Int:
                    case ShaderDataType::Int2:
                    case ShaderDataType::Int3:
                    case ShaderDataType::Int4:
                    case ShaderDataType::Bool:
                        BindIntAttribute(element, layout);
                        break;

                    case ShaderDataType::Mat3:
                    case ShaderDataType::Mat4:
                        BindMatrixAttribute(element, layout);
                        break;
                }
            }
            vertexBuffers.push_back(vertexBuffer);

            glBindVertexArray(0);
            vertexBuffer->Unbind();
        }
        void SetElementBuffer(ElementBuffer* elementBuffer) override 
        {
            glBindVertexArray(VAO);
            elementBuffer->Bind();

            this->elementBuffer = elementBuffer;

            glBindVertexArray(0);
            elementBuffer->Unbind();
        }
        const std::vector<VertexBuffer*>& GetVertexBuffers() const override
        {
            return vertexBuffers;
        }
        const ElementBuffer* GetElementBuffer() const override
        {
            return elementBuffer;
        }

    private:
        inline void BindFloatAttribute(const BufferElement& element, const BufferLayout& layout)
        {
            glEnableVertexAttribArray(attributeIndex);
            glVertexAttribPointer(
                attributeIndex,
                element.GetComponentCount(),
                Pitaya::Engine::Renderer::OpenGLShader::ShaderDataTypeToOpenGLBaseType(element.Type),
                element.Normalized ? GL_TRUE : GL_FALSE,
                layout.GetStride(),
                (const void*)(uintptr_t)element.Offset);
            glVertexAttribDivisor(attributeIndex, element.Instanced ? 1 : 0);
            attributeIndex++;
        }
        inline void BindIntAttribute(const BufferElement& element, const BufferLayout& layout)
        {
            glEnableVertexAttribArray(attributeIndex);
            glVertexAttribIPointer(
                attributeIndex,
                element.GetComponentCount(),
                Pitaya::Engine::Renderer::OpenGLShader::ShaderDataTypeToOpenGLBaseType(element.Type),
                layout.GetStride(),
                (const void*)(uintptr_t)element.Offset);
            glVertexAttribDivisor(attributeIndex, element.Instanced ? 1 : 0);
            attributeIndex++;
        }
        inline void BindMatrixAttribute(const BufferElement& element, const BufferLayout& layout)
        {
            uint8_t count = element.GetComponentCount();
            for (uint8_t i = 0; i < count; i++)
            {
                glEnableVertexAttribArray(attributeIndex);
                glVertexAttribPointer(
                    attributeIndex,
                    count,
                    Pitaya::Engine::Renderer::OpenGLShader::ShaderDataTypeToOpenGLBaseType(element.Type),
                    element.Normalized ? GL_TRUE : GL_FALSE,
                    layout.GetStride(),
                    (const void*)(uintptr_t)(element.Offset + sizeof(float) * count * i));
                glVertexAttribDivisor(attributeIndex, element.Instanced ? 1 : 0);
                attributeIndex++;
            }
        }

	private:
		uint32_t VAO = 0;
        uint32_t attributeIndex = 0;
        std::vector<VertexBuffer*> vertexBuffers;
		ElementBuffer* elementBuffer = nullptr;
	};
}