#pragma once

#include<Renderer/Runtime/Vertex/VertexBuffer.h>

#include<GL/glew.h>
#include<GLFW/glfw3.h>

#include<cstdint>

namespace Pitaya::Engine::Renderer
{
	class OpenGLVertexBuffer : public VertexBuffer
	{
	public:
		OpenGLVertexBuffer(float* vertices, uint32_t size)
		{
			glGenBuffers(1, &VBO);
			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
		}
		~OpenGLVertexBuffer() override
		{
			glDeleteBuffers(1, &VBO);
			VBO = 0;
		}

		void Bind() const override
		{
			glBindBuffer(GL_ARRAY_BUFFER, VBO);
		}
		void Unbind() const override
		{
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		}

		void SetLayout(const BufferLayout& layout) override 
		{ 
			this->layout = layout;
		}
		const BufferLayout& GetLayout() const override 
		{ 
			return layout;
		}

	private:
		uint32_t VBO = 0;
		BufferLayout layout;
	};
}