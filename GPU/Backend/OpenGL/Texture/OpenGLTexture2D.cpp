#include<GPU/Frontend/Texture/Texture2D.h>

#if defined(PITAYA_GRAPHICS_OPENGL)
#include<GL/glew.h>

Pitaya::GPU::Texture2D Pitaya::GPU::Texture2D::Factory::Create(unsigned char* data, int width, int height, int channels, bool isGenerateMipmap, bool isSRGB, bool isNearest)
{
	GLuint target = 0;
	glGenTextures(1, &target);
	glBindTexture(GL_TEXTURE_2D, target);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, isNearest ? GL_NEAREST : GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

	if (isGenerateMipmap)
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, isNearest ? GL_NEAREST : GL_LINEAR);
	}

	glBindTexture(GL_TEXTURE_2D, 0);
	return { target };
}
void Pitaya::GPU::Texture2D::Factory::Destroy(Pitaya::GPU::Texture2D texture)
{
	GLuint id = texture.Id;
	if (id > 0) { glDeleteTextures(1, &id); }
}
#endif

