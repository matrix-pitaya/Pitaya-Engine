#pragma once

#include<GPU/Frontend/Texture/Texture2DArray.h>
#include<GL/glew.h>

namespace Pitaya::GPU
{
    class OpenGLTexture2DArray : public Texture2DArray
    {
    public:
        OpenGLTexture2DArray(int width, int height, int layers, bool isDepth)
            : Texture2DArray(width, height, layers, isDepth)
        {
            glGenTextures(1, &target);
            glBindTexture(GL_TEXTURE_2D_ARRAY, target);

            if (isDepth)
            {
                // 为阴影贴图配置纹理格式 (深度格式)
                glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_DEPTH_COMPONENT32F, width, height, layers, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);

                // 阴影贴图通常需要 Linear 过滤，如果不使用 PCF，可以用 Nearest
                glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

                // 关键：解决阴影贴图边缘采样问题
                glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
                glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

                // 设置 Border 颜色为白色（深度 1.0），确保阴影视锥外不是阴影
                float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
                glTexParameterfv(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_BORDER_COLOR, borderColor);

                // 开启硬件阴影比较（Shader 内部使用 sampler2DArrayShadow 时必须）
                glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
                glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
            }
            else
            {
                // 普通颜色纹理阵列
                glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA8, width, height, layers, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
                glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);
            }

            glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
        }
        ~OpenGLTexture2DArray() override
        {
            if (target != 0)
            {
                glDeleteTextures(1, &target);
                target = 0;
            }
        }

    public:
        Identifier<Texture2DArray> GetGPUIdentifier() const override
        {
            return target;
        }

    private:
        GLuint target = 0;
    };
}
