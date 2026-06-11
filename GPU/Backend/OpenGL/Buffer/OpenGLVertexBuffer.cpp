#include<GPU/Frontend/Buffer/VertexBuffer.h>
#include<cstdint>

#if defined(PITAYA_GRAPHICS_OPENGL)
#include<GL/glew.h>


Pitaya::GPU::VertexBuffer Pitaya::GPU::VertexBuffer::Factory::Create(const float* vertices, uint32_t size, BufferLayout layout)
{
	GLuint VBO = 0;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	return { VBO, layout };
}
void Pitaya::GPU::VertexBuffer::Factory::Destroy(VertexBuffer vertexBuffer)
{
	GLuint VBO = vertexBuffer.Id;
	if (VBO > 0) { glDeleteBuffers(1, &VBO); }
}
#endif