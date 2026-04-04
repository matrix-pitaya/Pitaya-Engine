#pragma once

#include<GPU/Frontend/Buffer/VertexBuffer.h>

#include<GL/glew.h>
#include<GLFW/glfw3.h>

#include<cstdint>

namespace Pitaya::GPU
{
	class OpenGLVertexBuffer : public VertexBuffer
	{
	public:
		OpenGLVertexBuffer(float* vertices, uint32_t size)
			:VertexBuffer(vertices, size)
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

	public:
		void Bind() const override
		{
			glBindBuffer(GL_ARRAY_BUFFER, VBO);
		}
		void Unbind() const override
		{
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		}
		Pitaya::GPU::Identifier<VertexBuffer> GetGPUIdentifier() const override
		{
			return VBO;
		}

	private:
		GLuint VBO = 0;
	};
}
