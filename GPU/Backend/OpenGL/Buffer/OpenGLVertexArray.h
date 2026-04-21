#pragma once

#include<Core/Allocate/Allocate.h>

#include<GPU/Frontend/Buffer/VertexArray.h>
#include<GPU/Backend/OpenGL/Shader/OpenGLShader.h>
#include<GPU/Common/FuncTable.h>
#include<Log/Common/FuncTable.h>

#include<GL/glew.h>
#include<GLFW/glfw3.h>

#include<cstdint>
#include<vector>

namespace Pitaya::GPU
{
	class OpenGLVertexArray : public VertexArray
	{
    public:
        OpenGLVertexArray(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey)
            :VertexArray(passkey)
        {
            glGenVertexArrays(1, &VAO);
        }
        ~OpenGLVertexArray() override
        {
            glDeleteVertexArrays(1, &VAO);
            VAO = 0;

            for (auto vertexBuffer : vertexBuffers)
            {
                if (vertexBuffer && vertexBuffer->ReduceRefCount())
                {
                    if (!Pitaya::GPU::DestroyVertexBuffer(passkey, vertexBuffer->GetGPUIdentifier()))
                    {
                        PITAYA_DELETE(vertexBuffer);  //资源不在资源池 手动销毁
                        Pitaya::Log::Warning("destroy vertex buffer fail,in vertex array destructor");
                    }
                }
            }
            vertexBuffers.clear();

            if (indexBuffer && indexBuffer->ReduceRefCount())
            {
                if (!Pitaya::GPU::DestroyIndexBuffer(passkey, indexBuffer->GetGPUIdentifier()))
                {
                    PITAYA_DELETE(indexBuffer);     //资源不在资源池 手动销毁
                    Pitaya::Log::Warning("destroy index buffer fail,in vertex array destructor");
                }
            }
            indexBuffer = nullptr;
        }

    public:
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
            if (!vertexBuffer)
            {
                Pitaya::Log::Error("attempted to add null vertex buffer to vertex array");
                return;
            }

            glBindVertexArray(VAO);
            vertexBuffer->Bind();
            const auto& layout = vertexBuffer->GetLayout();
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
            vertexBuffer->AddRefCount();
            vertexBuffers.push_back(vertexBuffer);
        }
        void SetIndexBuffer(IndexBuffer* indexBuffer) override 
        {
            if (!indexBuffer)
            {
                Pitaya::Log::Error("attempted to set null index buffer to vertex array");
                return;
            }

            if (this->indexBuffer == indexBuffer) { return; }

            glBindVertexArray(VAO);
            indexBuffer->Bind();

            if (this->indexBuffer && this->indexBuffer->ReduceRefCount()) 
            { 
                if (!Pitaya::GPU::DestroyIndexBuffer(passkey, this->indexBuffer->GetGPUIdentifier()))
                {
					PITAYA_DELETE(this->indexBuffer);     //资源不在资源池 手动销毁
                    Pitaya::Log::Warning("destroy index buffer fail,in vertex array set index buffer");
                }
            }

            indexBuffer->AddRefCount();
            this->indexBuffer = indexBuffer;
        }

    public:
        Identifier<VertexArray> GetGPUIdentifier() const override
        {
            return VAO;
        }

    private:
        inline void BindFloatAttribute(const BufferElement& element, const BufferLayout& layout)
        {
            glEnableVertexAttribArray(element.Location);
            glVertexAttribPointer(
                element.Location,
                element.GetComponentCount(),
                OpenGLShader::ShaderVariableTypeToOpenGLBaseType(element.Type),
                element.Normalized ? GL_TRUE : GL_FALSE,
                layout.GetStride(),
                (const void*)(uintptr_t)element.Offset);
            glVertexAttribDivisor(element.Location, element.Instanced ? 1 : 0);
        }
        inline void BindIntAttribute(const BufferElement& element, const BufferLayout& layout)
        {
            glEnableVertexAttribArray(element.Location);
            glVertexAttribIPointer(
                element.Location,
                element.GetComponentCount(),
                OpenGLShader::ShaderVariableTypeToOpenGLBaseType(element.Type),
                layout.GetStride(),
                (const void*)(uintptr_t)element.Offset);
            glVertexAttribDivisor(element.Location, element.Instanced ? 1 : 0);
        }
        inline void BindMatrixAttribute(const BufferElement& element, const BufferLayout& layout)
        {
            uint8_t count = element.GetComponentCount();
            for (uint8_t i = 0; i < count; i++)
            {
                uint32_t currentLocation = element.Location + i;
                glEnableVertexAttribArray(currentLocation);
                glVertexAttribPointer(
                    currentLocation,
                    count,
                    OpenGLShader::ShaderVariableTypeToOpenGLBaseType(element.Type),
                    element.Normalized ? GL_TRUE : GL_FALSE,
                    layout.GetStride(),
                    (const void*)(uintptr_t)(element.Offset + sizeof(float) * count * i));
                glVertexAttribDivisor(currentLocation, element.Instanced ? 1 : 0);
            }
        }

	private:
        GLuint VAO = 0;
	};
}
