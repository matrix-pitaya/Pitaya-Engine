#pragma once

#include<GPU/Frontend/Buffer/FrameBuffer.h>

#include<string>
#include<exception>
#include<stdexcept>

#include<GL/glew.h>
#include<GLFW/glfw3.h>

namespace Pitaya::GPU
{
    class OpenGLFrameBuffer : public FrameBuffer
    {
    public:
        OpenGLFrameBuffer(const Pitaya::GPU::FrameBufferSpecification& spec)
            : FrameBuffer(spec)
        {
            Invalidate();
        }
        ~OpenGLFrameBuffer() override
        {
            CleanUp();
        }

        void Bind() const override
        {
            // 始终绑定用于渲染的 FBO (如果是 MSAA 模式，这里绑定的就是多重采样 FBO)
            glBindFramebuffer(GL_FRAMEBUFFER, FBO);
            glViewport(0, 0, specification.Width, specification.Height);
        }
        void Unbind() const override
        {
            // 核心逻辑：如果是多重采样，在解绑前需要“解析”(Resolve) 到普通纹理
            if (specification.Samples > 1)
            {
                // 读：MSAA FBO
                glBindFramebuffer(GL_READ_FRAMEBUFFER, FBO);
                // 写：普通 FBO (Intermediate)
                glBindFramebuffer(GL_DRAW_FRAMEBUFFER, intermediateFBO);

                // 执行 Blit (拷贝并合并像素)
                glBlitFramebuffer(
                    0, 0, specification.Width, specification.Height, // Source Rect
                    0, 0, specification.Width, specification.Height, // Dest Rect
                    GL_COLOR_BUFFER_BIT,
                    GL_NEAREST); // 由于尺寸完全一致，NEAREST 即可
            }

            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }

        void Resize(uint32_t width, uint32_t height) override
        {
            if (width == 0 || height == 0 || width > 8192 || height > 8192)
            {
                throw std::runtime_error("FrameBuffer Resize Warning: Attempted to resize to " + std::to_string(width) + ", " + std::to_string(height));
            }

            specification.Width = width;
            specification.Height = height;
            Invalidate();
        }

        Identifier<FrameBuffer> GetGPUIdentifier() const override
        {
            return FBO;
        }
        Identifier<FrameBuffer> GetInternalGPUIdentifier() const override
        {
            return intermediateFBO;
        }
        Identifier<Texture2D> GetColorAttachmentGPUIdentifier() const override
        {
            return specification.Samples > 1 ? intermediateColorAttachment : colorAttachment;
        }
        Identifier<Texture2D> GetDepthAttachmentGPUIdentifier() const override
        {
            return depthAttachment;
        }

    private:
        void Invalidate()
        {
            if (FBO) { CleanUp(); }

            bool multisample = specification.Samples > 1;

            GLenum internalFormat = specification.HDR ? GL_RGBA16F : GL_RGBA8;
            GLenum dataType = specification.HDR ? GL_FLOAT : GL_UNSIGNED_BYTE;

            // 创建主渲染 FBO
            glGenFramebuffers(1, &FBO);
            glBindFramebuffer(GL_FRAMEBUFFER, FBO);

            // 创建颜色附件
            glGenTextures(1, &colorAttachment);
            if (multisample)
            {
                glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, colorAttachment);
                glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, specification.Samples, internalFormat, specification.Width, specification.Height, GL_TRUE);
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, colorAttachment, 0);
            }
            else
            {
                glBindTexture(GL_TEXTURE_2D, colorAttachment);
                glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, specification.Width, specification.Height, 0, GL_RGBA, dataType, nullptr);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorAttachment, 0);
            }

            // 创建深度附件
            if (specification.HasDepth)
            {
                glGenTextures(1, &depthAttachment);
                if (multisample)
                {
                    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, depthAttachment);
                    glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, specification.Samples, GL_DEPTH24_STENCIL8, specification.Width, specification.Height, GL_TRUE);
                    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D_MULTISAMPLE, depthAttachment, 0);
                }
                else
                {
                    glBindTexture(GL_TEXTURE_2D, depthAttachment);
                    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, specification.Width, specification.Height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, nullptr);
                    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, depthAttachment, 0);
                }
            }

            if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            {
                CleanUp();
                throw std::runtime_error("Framebuffer Is Not Complete!");
            }

            // 如果开启 MSAA 创建中间 FBO (intermediateFBO)
            if (multisample)
            {
                glGenFramebuffers(1, &intermediateFBO);
                glBindFramebuffer(GL_FRAMEBUFFER, intermediateFBO);

                glGenTextures(1, &intermediateColorAttachment);
                glBindTexture(GL_TEXTURE_2D, intermediateColorAttachment);

                // 这里的 Resolve 纹理格式必须与主 FBO 保持一致 (HDR支持)
                glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, specification.Width, specification.Height, 0, GL_RGBA, dataType, nullptr);

                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, intermediateColorAttachment, 0);

                if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
                {
                    CleanUp();
                    throw std::runtime_error("Intermediate Framebuffer Is Not Complete!");
                }
            }

            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }
        void CleanUp()
        {
            if (FBO) { glDeleteFramebuffers(1, &FBO); FBO = 0; }
            if (colorAttachment) { glDeleteTextures(1, &colorAttachment); colorAttachment = 0; }
            if (depthAttachment) { glDeleteTextures(1, &depthAttachment); depthAttachment = 0; }

            // 清理中间 FBO 资源
            if (intermediateFBO) { glDeleteFramebuffers(1, &intermediateFBO); intermediateFBO = 0; }
            if (intermediateColorAttachment) { glDeleteTextures(1, &intermediateColorAttachment); intermediateColorAttachment = 0; }
        }

    private:
        // MSAA 模式下：这是多重采样 FBO
        // 普通模式下：这是普通 FBO
        GLuint FBO = 0;
        GLuint colorAttachment = 0;
        GLuint depthAttachment = 0;

        // 仅在 MSAA 模式下使用: 解析后的普通 FBO 用于显示
        GLuint intermediateFBO = 0;
        GLuint intermediateColorAttachment = 0;
    };
}
