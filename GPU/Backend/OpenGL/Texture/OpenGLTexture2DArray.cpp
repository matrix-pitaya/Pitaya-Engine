#include<GPU/Frontend/Texture/Texture2DArray.h>
#include<GPU/Backend/OpenGL/PixelFormatGL.h>

#if defined(PITAYA_GRAPHICS_OPENGL)
#include<GL/glew.h>

Pitaya::GPU::Texture2DArray Pitaya::GPU::Texture2DArray::Factory::Create(int width, int height, int layers, PixelFormat format)
{
    const auto triplet = Pitaya::GPU::PixelFormatToGL(format);
    const bool isDepth = Pitaya::GPU::PixelFormatIsDepth(format);

    GLuint target = 0;
    glGenTextures(1, &target);
    glBindTexture(GL_TEXTURE_2D_ARRAY, target);

    glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, triplet.InternalFormat, width, height, layers, 0, triplet.Format, triplet.Type, nullptr);

    if (isDepth)
    {
        // 阴影贴图常规：Linear 过滤 + Border + 硬件阴影比较
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // 解决阴影贴图边缘采样：边界外视为 1.0（非阴影）
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
        glTexParameterfv(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_BORDER_COLOR, borderColor);

        // 硬件阴影比较（sampler2DArrayShadow 必须）
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
    }
    else
    {
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);
    }

    glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
    return { target };
}
void Pitaya::GPU::Texture2DArray::Factory::Destroy(Pitaya::GPU::Texture2DArray texture2DArray)
{
    GLuint target = texture2DArray.Id;
    if (target != 0) { glDeleteTextures(1, &target); }
}
#endif
