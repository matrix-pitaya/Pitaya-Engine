#include<GPU/Frontend/Texture/TextureCubemap.h>
#include<GPU/Backend/OpenGL/PixelFormatGL.h>
#include<vector>

#if defined(PITAYA_GRAPHICS_OPENGL)
#include<GL/glew.h>

Pitaya::GPU::TextureCubemap Pitaya::GPU::TextureCubemap::Factory::Create(const void** datas, int* widths, int* heights, PixelFormat format, bool isGenerateMipmap, bool isNearest)
{
    const auto triplet = Pitaya::GPU::PixelFormatToGL(format);

    GLuint target = 0;
    glGenTextures(1, &target);
    glBindTexture(GL_TEXTURE_CUBE_MAP, target);

    for (int i = 0; i < 6; i++)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, triplet.InternalFormat, widths[i], heights[i], 0, triplet.Format, triplet.Type, datas[i]);
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
    return { target };
}
Pitaya::GPU::TextureCubemap Pitaya::GPU::TextureCubemap::Factory::Create(int size, int mipLevels, PixelFormat format)
{
    const auto triplet = Pitaya::GPU::PixelFormatToGL(format);

    GLuint target = 0;
    glGenTextures(1, &target);
    glBindTexture(GL_TEXTURE_CUBE_MAP, target);

    // 一次性分配 6 面 × mipLevels 的存储（不可变存储）
    glTexStorage2D(GL_TEXTURE_CUBE_MAP, mipLevels, triplet.InternalFormat, size, size);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, mipLevels > 1 ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    return { target };
}
void  Pitaya::GPU::TextureCubemap::Factory::Destroy(Pitaya::GPU::TextureCubemap textureCubemap)
{
    GLuint target = textureCubemap.Id;
    if (target > 0) { glDeleteTextures(1, &target); }
}
#endif
