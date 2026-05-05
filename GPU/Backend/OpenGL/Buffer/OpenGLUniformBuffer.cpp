#include<GPU/Frontend/Buffer/UniformBuffer.h>

#if defined(PITAYA_GRAPHICS_OPENGL)
#include<GL/glew.h>

Pitaya::GPU::UniformBuffer Pitaya::GPU::UniformBuffer::Factory::Create(uint32_t size, uint32_t bindingPoint)
{
	GLuint UBO = 0;
	glGenBuffers(1, &UBO);
	glBindBuffer(GL_UNIFORM_BUFFER, UBO);
	glBufferData(GL_UNIFORM_BUFFER, size, NULL, GL_DYNAMIC_DRAW); // 动态更新
	glBindBufferBase(GL_UNIFORM_BUFFER, bindingPoint, UBO);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	return { UBO, size, bindingPoint };
}
void Pitaya::GPU::UniformBuffer::Factory::Destroy(Pitaya::GPU::UniformBuffer uniformBuffer)
{
	GLuint UBO = uniformBuffer.Id;
	if (UBO > 0) { glDeleteBuffers(1, &UBO); }
}
#endif