#pragma once

#include<Renderer/Runtime/Vertex/VertexBuffer.h>
#include<Renderer/Runtime/Vertex/ElementBuffer.h>

#include<vector>

namespace Pitaya::Engine::Renderer
{
	class VertexArray
	{
	public:
        VertexArray() = default;
        virtual ~VertexArray() = default;

        virtual void Bind() const = 0;
        virtual void Unbind() const = 0;

        virtual void AddVertexBuffer(VertexBuffer* vertexBuffer) = 0;
        virtual void SetElementBuffer(ElementBuffer* elementBuffer) = 0;

        virtual const std::vector<VertexBuffer*>& GetVertexBuffers() const = 0;
        virtual const ElementBuffer* GetElementBuffer() const = 0;
	};
}