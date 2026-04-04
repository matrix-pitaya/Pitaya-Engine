#pragma once

#include<GPU/Frontend/Texture/TextureCubemap.h>

#include<GL/glew.h>
#include<GLFW/glfw3.h>

#include<vector>

namespace Pitaya::GPU
{
	class OpenGLTextureCubemap : public TextureCubemap
	{
	public:
        OpenGLTextureCubemap(unsigned char** datas,
            int* widths, int* heights, int* channels, bool isGenerateMipmap, bool isSRGB, bool isNearest)
            :TextureCubemap(datas, widths, heights, channels, isGenerateMipmap, isSRGB, isNearest)
        {
            glGenTextures(1, &target);
            glBindTexture(GL_TEXTURE_CUBE_MAP, target);

            for (int i = 0; i < 6; i++)
            {
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA8, widths[i], heights[i], 0, GL_RGBA, GL_UNSIGNED_BYTE, datas[i]);
            }

            if (isGenerateMipmap)
            {
                glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
                glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
            }
            else
            {
                glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, isNearest ? GL_NEAREST : GL_LINEAR);
            }

            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, isNearest ? GL_NEAREST : GL_LINEAR);

            glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
        }
		~OpenGLTextureCubemap() override
		{
			glDeleteTextures(1, &target);
			target = 0;
		}

		Identifier<TextureCubemap> GetGPUIdentifier() const override
		{
			return target;
		}

	private:
		GLuint target = 0;
	};
}
