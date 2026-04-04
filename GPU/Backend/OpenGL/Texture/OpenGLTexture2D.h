#pragma once

#include<GPU/Frontend/Texture/Texture2D.h>

#include<GL/glew.h>
#include<GLFW/glfw3.h>

namespace Pitaya::GPU
{
	class OpenGLTexture2D : public Texture2D
	{
	public:
		OpenGLTexture2D(unsigned char* data, int width, int height, int channels, bool isGenerateMipmap, bool isSRGB, bool isNearest)
			:Texture2D(data, width, height, channels, isGenerateMipmap, isSRGB, isNearest)
		{
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
		}
		~OpenGLTexture2D() override
		{
			glDeleteTextures(1, &target);
			target = 0;
		}

	public:
		Identifier<Texture2D> GetGPUIdentifier() const override
		{
			return target;
		}

	private:
		GLuint target = 0;
	};
}
