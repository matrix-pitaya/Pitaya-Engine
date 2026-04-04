#pragma once

#include<GPU/Frontend/Buffer/UniformBuffer.h>

#include<GL/glew.h>
#include<GLFW/glfw3.h>

namespace Pitaya::GPU
{
	class OpenGLUniformBuffer : public UniformBuffer
	{
	public:
		OpenGLUniformBuffer(uint32_t size, uint32_t bindingPoint)
			:UniformBuffer(size, bindingPoint)
		{
			glGenBuffers(1, &UBO);
			glBindBuffer(GL_UNIFORM_BUFFER, UBO);
			glBufferData(GL_UNIFORM_BUFFER, size, NULL, GL_DYNAMIC_DRAW); // 动态更新
			glBindBufferBase(GL_UNIFORM_BUFFER, bindingPoint, UBO);
			glBindBuffer(GL_UNIFORM_BUFFER, 0);
		}
		~OpenGLUniformBuffer() override
		{
			glDeleteBuffers(1, &UBO);
			UBO = 0;
		}

	public:
		void SetData(const void* data, uint32_t size, uint32_t offset = 0) const override
		{
			glBindBuffer(GL_UNIFORM_BUFFER, UBO);
			glBufferSubData(GL_UNIFORM_BUFFER, offset, size, data);
			glBindBuffer(GL_UNIFORM_BUFFER, 0);
		}
		Identifier<UniformBuffer> GetGPUIdentifier() const override
		{
			return UBO;
		}

	private:
		GLuint UBO = 0;
	};
}
