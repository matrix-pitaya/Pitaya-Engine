#pragma once

#include<Renderer/Runtime/Vertex/ElementBuffer.h>

#include<GL/glew.h>
#include<GLFW/glfw3.h>

namespace Pitaya::Engine::Renderer
{
	class OpenGLElementBuffer : public ElementBuffer
	{
	public:
		OpenGLElementBuffer(uint32_t* indices, uint32_t count)
			: count(count)
		{
			glGenBuffers(1, &EBO);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);		//EBO的绑定必须在VAO绑定的情况下才能正确记录
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(uint32_t), indices, GL_STATIC_DRAW);
		}
		~OpenGLElementBuffer() override
		{
			glDeleteBuffers(1, &EBO);
			EBO = 0;
			count = 0;
		}

		void Bind() const override 
		{ 
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		}
		void Unbind() const override
		{ 
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); 
		}
		uint32_t GetCount() const override
		{ 
			return count;
		}

	private:
		uint32_t EBO = 0;
		uint32_t count = 0;
	};
}