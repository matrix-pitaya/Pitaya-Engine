#pragma once

#include<GPU/Frontend/Buffer/VertexBuffer.h>
#include<GPU/Frontend/Buffer/IndexBuffer.h>

#include<vector>

namespace Pitaya::GPU
{
    class RHIDevice;
	class VertexArray
	{
        friend class Pitaya::GPU::RHIDevice;
	public:
        VertexArray() = default;
        virtual ~VertexArray() = default;

    public:
        virtual void Bind() const = 0;
        virtual void Unbind() const = 0;

        virtual void AddVertexBuffer(VertexBuffer* vertexBuffer) = 0;
        virtual void SetIndexBuffer(IndexBuffer* indexBuffer) = 0;
        virtual Identifier<VertexArray> GetGPUIdentifier() const = 0;

    public:
        inline const std::vector<VertexBuffer*>& GetVertexBuffers() const
        {
            return vertexBuffers;
        }
        inline const IndexBuffer* GetElementBuffer() const
        {
            return indexBuffer;
        }

    private:
        static VertexArray* Create();

    protected:
        std::vector<VertexBuffer*> vertexBuffers;
        IndexBuffer* indexBuffer = nullptr;
	};
}
