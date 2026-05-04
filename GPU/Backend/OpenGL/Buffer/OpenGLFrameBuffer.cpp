#include<GPU/Frontend/Buffer/FrameBuffer.h>
#include<string>
#include<exception>
#include<stdexcept>

#if defined(PITAYA_USE_OPENGL)
#include<GL/glew.h>
#include<GLFW/glfw3.h>

Pitaya::GPU::FrameBuffer Pitaya::GPU::FrameBuffer::Factory::Create(Pitaya::GPU::FrameBufferSpecification specification)
{
    // MSAA 模式下：这是多重采样 FBO
    // 普通模式下：这是普通 FBO
    GLuint FBO = 0;
    GLuint colorAttachment = 0;
    GLuint depthAttachment = 0;

    // 仅在 MSAA 模式下使用: 解析后的普通 FBO 用于显示
    GLuint intermediateFBO = 0;
    GLuint intermediateColorAttachment = 0;

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
        Pitaya::GPU::FrameBuffer::Factory::Destroy({ FBO, intermediateFBO, colorAttachment, intermediateColorAttachment, depthAttachment, specification });
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
            Pitaya::GPU::FrameBuffer::Factory::Destroy({ FBO, intermediateFBO, colorAttachment, intermediateColorAttachment, depthAttachment, specification });
            throw std::runtime_error("Intermediate Framebuffer Is Not Complete!");
        }
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return { FBO, intermediateFBO, colorAttachment, intermediateColorAttachment, depthAttachment, specification };
}
void Pitaya::GPU::FrameBuffer::Factory::Destroy(Pitaya::GPU::FrameBuffer frameBuffer)
{ 
    // MSAA 模式下：这是多重采样 FBO
    // 普通模式下：这是普通 FBO
    GLuint FBO = frameBuffer.Id;
    GLuint colorAttachment = frameBuffer.ColorAttachmentId;
    GLuint depthAttachment = frameBuffer.DepthAttachmentId;

    // 仅在 MSAA 模式下使用: 解析后的普通 FBO 用于显示
    GLuint intermediateFBO = frameBuffer.InternalId;
    GLuint intermediateColorAttachment = frameBuffer.InternalColorAttachmentId;

    if (FBO) { glDeleteFramebuffers(1, &FBO); FBO = 0; }
    if (colorAttachment) { glDeleteTextures(1, &colorAttachment); colorAttachment = 0; }
    if (depthAttachment) { glDeleteTextures(1, &depthAttachment); depthAttachment = 0; }

    // 清理中间 FBO 资源
    if (intermediateFBO) { glDeleteFramebuffers(1, &intermediateFBO); intermediateFBO = 0; }
    if (intermediateColorAttachment) { glDeleteTextures(1, &intermediateColorAttachment); intermediateColorAttachment = 0; }
}
#endif