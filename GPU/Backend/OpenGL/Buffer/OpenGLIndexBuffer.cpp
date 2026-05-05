#include<GPU/Frontend/Buffer/IndexBuffer.h>

#if defined(PITAYA_GRAPHICS_OPENGL)
#include<GL/glew.h>

Pitaya::GPU::IndexBuffer Pitaya::GPU::IndexBuffer::Factory::Create(uint32_t* indices, uint32_t count)
{
	GLuint EBO = 0;
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);		//EBO的绑定必须在VAO绑定的情况下才能正确记录
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(uint32_t), indices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	return { EBO ,count };
}
void Pitaya::GPU::IndexBuffer::Factory::Destroy(Pitaya::GPU::IndexBuffer indexBuffer)
{
	GLuint EBO = indexBuffer.Id;
	if (EBO > 0){ glDeleteBuffers(1, &EBO); }
}
#endif