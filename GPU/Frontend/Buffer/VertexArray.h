#pragma once

#include<Core/PassKey/PassKey.h>
#include<GPU/Frontend/Buffer/VertexBuffer.h>
#include<GPU/Frontend/Buffer/IndexBuffer.h>

#include<vector>

namespace Pitaya::Render { class Renderer; }

namespace Pitaya::GPU
{
    class RHIDevice;
	class VertexArray
	{
        friend class Pitaya::GPU::RHIDevice;
    public:
        VertexArray(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey)
            :passkey(passkey) { }
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
        static VertexArray* Create(Pitaya::Core::PassKey<Pitaya::Render::Renderer>);

    protected:
        std::vector<VertexBuffer*> vertexBuffers;
        IndexBuffer* indexBuffer = nullptr;

        //用于调用GPU::Destroy VBO和IBO [TODO 该考虑重构了]
        Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey;
	};
}
