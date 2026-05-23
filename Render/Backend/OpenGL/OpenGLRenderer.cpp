#include<Render/Frontend/Renderer.h>
#include<GPU/Common/BindPoint.h>
#include<GPU/Frontend/Buffer/ShaderStorageBuffer.h>
#include<GPU/Frontend/Buffer/UniformBuffer.h>
#include<GPU/Frontend/Texture/Texture2D.h>
#include<GPU/Frontend/Texture/Texture2DArray.h>
#include<GPU/Frontend/Texture/TextureCubemap.h>
#include<GPU/Common/TextureSlot.h>
#include<Log/Common/FuncTable.h>

#include<gtc/type_ptr.hpp>
#include<stdexcept>
#include<string>

#if defined(PITAYA_GRAPHICS_OPENGL) && defined(PITAYA_WINDOW_GLFW)
#include<GL/glew.h>
#include<GLFW/glfw3.h>

namespace
{
    struct OpenGLGraphicsContext
    {
        GLFWwindow* Window = nullptr;
    };
}

bool Pitaya::Render::Renderer::InitializeRenderContext(void* nativeWindow)
{
    auto glfwWindow = reinterpret_cast<GLFWwindow*>(nativeWindow);
    if (!glfwWindow)
    {
        Pitaya::Core::PopMessageBox("Error", "Initialize OpenGL Context Failed! Reinterpret Cast NativeWindow Fail!");
        Pitaya::Core::Terminate(-1);
    }

    //创建OpenGL上下文
    glfwMakeContextCurrent(glfwWindow);
    glfwSwapInterval(1);

    //初始化GLEW
    if (glewInit() != GLEW_OK)
    {
        Pitaya::Core::PopMessageBox("Error", "Initialize OpenGL Context Failed! GLEW InitFail!");
        Pitaya::Core::Terminate(-1);
    }

    glEnable(GL_DEPTH_TEST);					//开启深度测试
    glDepthFunc(GL_LEQUAL);
    glDepthMask(GL_TRUE);
    glEnable(GL_CULL_FACE);						//开启面剔除
    glCullFace(GL_BACK);						//剔除背面看不见的多边形
    glEnable(GL_STENCIL_TEST);					//开启模板测试
    glStencilFunc(GL_ALWAYS, 1, 0xFF);			//设置总是通过模板测试
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);	//设置更新模板缓冲区方式
    glStencilMask(0xFF);						//设置允许写入模板缓冲区
    glEnable(GL_MULTISAMPLE);					//启用多重采样抗锯齿
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);		//cubemap 跨面双线性
    //glEnable(GL_FRAMEBUFFER_SRGB);			//开启SRGB帧缓冲区进行Gamma矫正（通过后处理进行矫正）

    backendStorage.Cast<OpenGLGraphicsContext>().Window = glfwWindow;
    return true;
}
void Pitaya::Render::Renderer::ReleaseRenderContext()
{
    backendStorage.Cast<OpenGLGraphicsContext>().Window = nullptr;
}

void Pitaya::Render::Renderer::NewRenderFrame()
{
    Pitaya::GPU::ShaderStorageBuffer shaderStorageBuffer;

    // 动态扩容/上传 Transform SSBO
    size_t uploadTransformCount = renderPacket.back.InstanceInfo.size();
    if (uploadTransformCount > 0)
    {
        if (Pitaya::GPU::GetShaderStorageBuffer(Pitaya::Core::PassKey<Renderer>(), renderKit.InstanceModelTransformSSBO.Handle, shaderStorageBuffer))
        {
            size_t requiredSize = uploadTransformCount * sizeof(InstanceInfo);
            glBindBufferBase(GL_SHADER_STORAGE_BUFFER,
                static_cast<uint32_t>(Pitaya::GPU::SSBOBindPoint::InstanceModelTransform),
                shaderStorageBuffer.Id);

            // 检查预申请的显存能不能装下当前帧的数据
            if (requiredSize > renderKit.InstanceModelTransformSSBO.Capacity)
            {
                // 1.5 倍扩容策略
                renderKit.InstanceModelTransformSSBO.Capacity = requiredSize + (requiredSize / 2);
                glNamedBufferData(shaderStorageBuffer.Id, renderKit.InstanceModelTransformSSBO.Capacity, nullptr, GL_DYNAMIC_DRAW);	// 使用 glNamedBufferData 重新分配显存块
            }

            // 将最新推算好的数据安全更新到 SSBO 中
            glNamedBufferSubData(shaderStorageBuffer.Id, 0, requiredSize, renderPacket.back.InstanceInfo.data());
        }
    }

    // 动态扩容/上传 Bone SSBO
    size_t uploadBoneCount = renderPacket.back.BoneMatrices.size();
    if (uploadBoneCount > 0)
    {
        if (Pitaya::GPU::GetShaderStorageBuffer(Pitaya::Core::PassKey<Renderer>(), renderKit.BoneInverseMatriceSSBO.Handle, shaderStorageBuffer))
        {
            size_t requiredBoneSize = uploadBoneCount * sizeof(glm::mat4);
            glBindBufferBase(GL_SHADER_STORAGE_BUFFER,
                static_cast<uint32_t>(Pitaya::GPU::SSBOBindPoint::BoneInverseMatrice),
                shaderStorageBuffer.Id);

            if (requiredBoneSize > renderKit.BoneInverseMatriceSSBO.Capacity)
            {
                // 1.5倍扩容
                renderKit.BoneInverseMatriceSSBO.Capacity = requiredBoneSize + (requiredBoneSize / 2);
                glNamedBufferData(shaderStorageBuffer.Id, renderKit.BoneInverseMatriceSSBO.Capacity, nullptr, GL_DYNAMIC_DRAW);
            }

            glNamedBufferSubData(shaderStorageBuffer.Id, 0, requiredBoneSize, renderPacket.back.BoneMatrices.data());
        }
    }

    // 动态扩容/上传 Material SSBO
    size_t uploadMaterialSize = renderPacket.back.MaterialParams.size();
    if (uploadMaterialSize > 0)
    {
        if (Pitaya::GPU::GetShaderStorageBuffer(Pitaya::Core::PassKey<Renderer>(), renderKit.MaterialSSBO.Handle, shaderStorageBuffer))
        {
            glBindBufferBase(GL_SHADER_STORAGE_BUFFER,
                static_cast<uint32_t>(Pitaya::GPU::SSBOBindPoint::Material),
                shaderStorageBuffer.Id);

            if (uploadMaterialSize > renderKit.MaterialSSBO.Capacity)
            {
                renderKit.MaterialSSBO.Capacity = uploadMaterialSize + (uploadMaterialSize / 2);
                glNamedBufferData(shaderStorageBuffer.Id, renderKit.MaterialSSBO.Capacity, nullptr, GL_DYNAMIC_DRAW);
            }

            glNamedBufferSubData(shaderStorageBuffer.Id, 0, uploadMaterialSize, renderPacket.back.MaterialParams.data());

            // 将Texture2D Handle 转化为 Texture2D SamplerId 用于在 Shader 中通过 sampler采样纹理数据
            for (uint32_t offset : renderPacket.back.MaterialTexturePatches)
            {
                Pitaya::Core::SlotMap<Pitaya::GPU::Texture2D>::Handle handle;
                std::memcpy(&handle, renderPacket.back.MaterialParams.data() + offset, sizeof(handle));
                Pitaya::GPU::Texture2D tex;
                if (Pitaya::GPU::GetTexture2D(Pitaya::Core::PassKey<Pitaya::Render::Renderer>(), handle, tex))
                {
                    uint64_t samplerId = tex.SamplerId;
                    glNamedBufferSubData(shaderStorageBuffer.Id, offset, sizeof(uint64_t), &samplerId);
                }
            }
        }
    }

    // 动态扩容 Light SSBO（仅扩容，不在此处上传数据，由 ExecuteCommand(BeginPassCommand) 每 Pass 上传）
    size_t totalLightCount = renderPacket.back.Lights.size();
    size_t headerSize = 4 * sizeof(uint32_t);
    size_t maxRequiredSize = headerSize + totalLightCount * sizeof(Pitaya::Render::LightInfo);
    if (Pitaya::GPU::GetShaderStorageBuffer(Pitaya::Core::PassKey<Renderer>(), renderKit.SceneLightsSSBO.Handle, shaderStorageBuffer))
    {
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER,
            static_cast<uint32_t>(Pitaya::GPU::SSBOBindPoint::SceneLights),
            shaderStorageBuffer.Id);

        if (maxRequiredSize > renderKit.SceneLightsSSBO.Capacity)
        {
            renderKit.SceneLightsSSBO.Capacity = maxRequiredSize + (maxRequiredSize / 2);
            glNamedBufferData(shaderStorageBuffer.Id, renderKit.SceneLightsSSBO.Capacity, nullptr, GL_DYNAMIC_DRAW);
        }
    }

    // 动态扩容/上传 Shadow Atlas
    auto EnsureAtlas = [](RenderKit::ShadowAtlas& atlas, uint32_t requiredLayers, uint32_t resolution)
        {
            if (requiredLayers == 0 || requiredLayers <= atlas.LayerCapacity) { return; }

            uint32_t newCapacity = requiredLayers + requiredLayers / 2;
            if (newCapacity < 4) { newCapacity = 4; }

            if (atlas.LayerCapacity > 0)
            {
                Pitaya::GPU::DestroyTexture2DArray(
                    Pitaya::Core::PassKey<Pitaya::Render::Renderer>(),
                    atlas.TextureHandle);
            }

            atlas.TextureHandle = Pitaya::GPU::CreateTexture2DArray(
                Pitaya::Core::PassKey<Pitaya::Render::Renderer>(),
                static_cast<int>(resolution),
                static_cast<int>(resolution),
                static_cast<int>(newCapacity), Pitaya::GPU::PixelFormat::Depth32F);
            atlas.LayerCapacity = newCapacity;
        };
    EnsureAtlas(renderKit.CSMAtlas, renderPacket.back.RequiredCSMLayers, RenderKit::ShadowAtlas::CSMResolution);
    EnsureAtlas(renderKit.SpotShadowAtlas, renderPacket.back.RequiredSpotLayers, RenderKit::ShadowAtlas::SpotResolution);
    EnsureAtlas(renderKit.PointShadowAtlas, renderPacket.back.RequiredPointLayers, RenderKit::ShadowAtlas::PointResolution);
    if (!renderPacket.back.ShadowSSBOData.empty())
    {
        if (Pitaya::GPU::GetShaderStorageBuffer(Pitaya::Core::PassKey<Renderer>(),
            renderKit.ShadowSSBO.Handle, shaderStorageBuffer))
        {
            size_t requiredSize = renderPacket.back.ShadowSSBOData.size();

            glBindBufferBase(GL_SHADER_STORAGE_BUFFER,
                static_cast<uint32_t>(Pitaya::GPU::SSBOBindPoint::Shadow),
                shaderStorageBuffer.Id);

            if (requiredSize > renderKit.ShadowSSBO.Capacity)
            {
                renderKit.ShadowSSBO.Capacity = requiredSize + (requiredSize / 2);
                glNamedBufferData(shaderStorageBuffer.Id,
                    static_cast<GLsizeiptr>(renderKit.ShadowSSBO.Capacity),
                    nullptr, GL_DYNAMIC_DRAW);
            }

            glNamedBufferSubData(shaderStorageBuffer.Id, 0,
                static_cast<GLsizeiptr>(requiredSize),
                renderPacket.back.ShadowSSBOData.data());
        }
    }
    auto BindShadowAtlas = [](const RenderKit::ShadowAtlas& atlas, uint32_t unit)
        {
            if (atlas.LayerCapacity == 0) { return; }
            Pitaya::GPU::Texture2DArray tex;
            if (Pitaya::GPU::GetTexture2DArray(Pitaya::Core::PassKey<Pitaya::Render::Renderer>(), atlas.TextureHandle, tex))
            {
                glActiveTexture(GL_TEXTURE0 + unit);
                glBindTexture(GL_TEXTURE_2D_ARRAY, tex.Id);
            }
        };
    BindShadowAtlas(renderKit.CSMAtlas, static_cast<uint32_t>(Pitaya::GPU::TextureSlot::CSM));
    BindShadowAtlas(renderKit.SpotShadowAtlas, static_cast<uint32_t>(Pitaya::GPU::TextureSlot::SPOT));
    BindShadowAtlas(renderKit.PointShadowAtlas, static_cast<uint32_t>(Pitaya::GPU::TextureSlot::POINT));

    // 上传 SceneInfo UBO
    Pitaya::Render::SceneInfo sceneInfo = {};
    const auto& setup = renderPacket.back.SceneInfoSetup;
    sceneInfo.AmbientColor = setup.AmbientColor;
    sceneInfo.DeltaTime = setup.DeltaTime;
    Pitaya::GPU::TextureCubemap cubemap;
    if (Pitaya::GPU::GetTextureCubemap(Pitaya::Core::PassKey<Pitaya::Render::Renderer>(), setup.EnvCubemapHandle, cubemap))
        { sceneInfo.EnvCubemapHandle = cubemap.SamplerId; }
    if (Pitaya::GPU::GetTextureCubemap(Pitaya::Core::PassKey<Pitaya::Render::Renderer>(), setup.IrradianceHandle, cubemap))
        { sceneInfo.IrradianceHandle = cubemap.SamplerId; }
    if (Pitaya::GPU::GetTextureCubemap(Pitaya::Core::PassKey<Pitaya::Render::Renderer>(), setup.PrefilteredHandle, cubemap))
        { sceneInfo.PrefilteredHandle = cubemap.SamplerId; }
    Pitaya::GPU::Texture2D tex2D;
    if (Pitaya::GPU::GetTexture2D(Pitaya::Core::PassKey<Pitaya::Render::Renderer>(), setup.BRDFLUTHandle, tex2D))
        { sceneInfo.BRDFLUTHandle = tex2D.SamplerId; }
    Pitaya::GPU::UniformBuffer ubo;
    if (Pitaya::GPU::GetUniformBuffer(Pitaya::Core::PassKey<Pitaya::Render::Renderer>(), renderKit.SceneInfoUBO.Handle, ubo))
    {
        glNamedBufferSubData(ubo.Id, 0, sizeof(Pitaya::Render::SceneInfo), &sceneInfo);
    }
}
void Pitaya::Render::Renderer::SwapBuffer() const
{
    glfwSwapBuffers(backendStorage.Cast<OpenGLGraphicsContext>().Window);
}
void Pitaya::Render::Renderer::WaitFence() const
{
    GLsync fence = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
    glClientWaitSync(fence, GL_SYNC_FLUSH_COMMANDS_BIT, GL_TIMEOUT_IGNORED);
    glDeleteSync(fence);
}

void Pitaya::Render::Renderer::ExecuteCommand(const Pitaya::Render::BeginShadowPassCommand* command) const
{
    if (!command) { return; }

    // 选择 atlas
    Pitaya::Core::SlotMap<Pitaya::GPU::Texture2DArray>::Handle atlasTextureHandle;
    uint32_t LayerCapacity = 0;
    switch (command->LightType)
    {
        case 0:
            atlasTextureHandle = renderKit.CSMAtlas.TextureHandle;
            LayerCapacity = renderKit.CSMAtlas.LayerCapacity;
            break;

        case 1:
            atlasTextureHandle = renderKit.PointShadowAtlas.TextureHandle;
            LayerCapacity = renderKit.PointShadowAtlas.LayerCapacity;
            break;

        case 2:
            atlasTextureHandle = renderKit.SpotShadowAtlas.TextureHandle;
            LayerCapacity = renderKit.SpotShadowAtlas.LayerCapacity;
            break;

        default: return;
    }
    if (LayerCapacity == 0) { return; }

    // 通过 Handle 获取 Texture2DArray 和 Shadow FBO
    Pitaya::GPU::Texture2DArray texArray;
    Pitaya::GPU::FrameBuffer shadowFbo;
    if (Pitaya::GPU::GetTexture2DArray(Pitaya::Core::PassKey<Pitaya::Render::Renderer>(), atlasTextureHandle, texArray) &&
        Pitaya::GPU::GetFrameBuffer(Pitaya::Core::PassKey<Pitaya::Render::Renderer>(), renderKit.Specific.ShadowFBO, shadowFbo))
    {
        // 绑定 Shadow FBO + 挂指定 layer 为 depth attachment
        glBindFramebuffer(GL_FRAMEBUFFER, shadowFbo.Id);
        glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
            texArray.Id, 0, static_cast<GLint>(command->Layer));
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);

        // Viewport + Clear
        glViewport(0, 0, static_cast<GLsizei>(command->Resolution), static_cast<GLsizei>(command->Resolution));
        glClear(GL_DEPTH_BUFFER_BIT);

        // 渲染状态：深度写入，无颜色写入，front-face culling
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        glDepthMask(GL_TRUE);
        glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_FRONT);

        // 禁用模板（shadow pass 不需要）
        glDisable(GL_STENCIL_TEST);

        // 上传 shadow VP 到 CameraSnapshot UBO
        struct ShadowCameraBlock
        {
            glm::mat4 View;
            glm::mat4 Projection;
            glm::mat4 ViewProjection;
            glm::vec4 Position;
        };

        ShadowCameraBlock camBlock;
        camBlock.View = glm::mat4(1.0f);
        camBlock.Projection = glm::mat4(1.0f);
        camBlock.ViewProjection = command->ShadowViewProjection;
        camBlock.Position = glm::vec4(0.0f);

        Pitaya::GPU::UniformBuffer cameraUbo;
        if (Pitaya::GPU::GetUniformBuffer(Pitaya::Core::PassKey<Pitaya::Render::Renderer>(), renderKit.CameraSnapshotUBO.Handle, cameraUbo))
        {
            glNamedBufferSubData(cameraUbo.Id, 0, sizeof(ShadowCameraBlock), &camBlock);
        }
    }
}
void Pitaya::Render::Renderer::ExecuteCommand(const Pitaya::Render::DrawSkyboxCommand* command) const
{
    if (!command) { return; }

    Pitaya::GPU::Shader skyboxShader;
    if (!Pitaya::GPU::GetShader(Pitaya::Core::PassKey<Pitaya::Render::Renderer>(), renderKit.SkyBox.ShaderHandle, skyboxShader))
    { return; }

    glUseProgram(skyboxShader.Id);
    glDepthFunc(GL_LEQUAL);
    glDisable(GL_CULL_FACE);

    Pitaya::GPU::VertexArray vao;
    if (Pitaya::GPU::GetVertexArray(Pitaya::Core::PassKey<Pitaya::Render::Renderer>(), renderKit.Fallback.VAOHandle, vao))  // Fallback VAO 为立方体
    {
        glBindVertexArray(vao.Id);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);
    }

    glEnable(GL_CULL_FACE);
}
void Pitaya::Render::Renderer::ExecuteCommand(const Pitaya::Render::BeginPassCommand* command) const
{
    if (!command) { return; }

    // 恢复可能被 shadow pass 修改的状态
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glCullFace(GL_BACK);
    glEnable(GL_STENCIL_TEST);
    glStencilFunc(GL_ALWAYS, 1, 0xFF);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    glStencilMask(0xFF);

    // 更新 Camera UBO
    Pitaya::GPU::UniformBuffer cameraUbo;
    if (Pitaya::GPU::GetUniformBuffer(Pitaya::Core::PassKey<Renderer>(), renderKit.CameraSnapshotUBO.Handle, cameraUbo))
    {
        glNamedBufferSubData(cameraUbo.Id, 0, sizeof(Pitaya::Core::CameraSnapshot), &command->CameraSnapshot);
    }

    // 每 Pass 刷新光源 SSBO
    Pitaya::GPU::ShaderStorageBuffer lightsSSBO;
    if (Pitaya::GPU::GetShaderStorageBuffer(Pitaya::Core::PassKey<Renderer>(), renderKit.SceneLightsSSBO.Handle, lightsSSBO))
    {
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER,
            static_cast<uint32_t>(Pitaya::GPU::SSBOBindPoint::SceneLights),
            lightsSSBO.Id);

        // 写入 Header (16 字节对齐：1个有效uint + 3个padding)
        uint32_t lightHeader[4] = { command->LightCount, 0, 0, 0 };
        glNamedBufferSubData(lightsSSBO.Id, 0, sizeof(lightHeader), lightHeader);

        // 写入该 Pass 对应的光源数据段
        if (command->LightCount > 0)
        {
            size_t headerSize = 4 * sizeof(uint32_t);
            size_t dataSize = command->LightCount * sizeof(Pitaya::Render::LightInfo);
            const Pitaya::Render::LightInfo* passLights = &renderPacket.back.Lights[command->LightDataOffset];
            glNamedBufferSubData(lightsSSBO.Id, headerSize, dataSize, passLights);
        }
    }

    // FBO 切换与 Clear
    Pitaya::GPU::FrameBuffer fbo;
    if (Pitaya::GPU::GetFrameBuffer(Pitaya::Core::PassKey<Renderer>(), command->SceneFrameBufferHandle, fbo))
    {
        glBindFramebuffer(GL_FRAMEBUFFER, fbo.Id);
        glViewport(command->Rect.Position.x, command->Rect.Position.y, command->Rect.Size.x, command->Rect.Size.y);
        glClearColor(command->ClearColor.r, command->ClearColor.g, command->ClearColor.b, command->ClearColor.a);

        GLbitfield clearBit = GL_COLOR_BUFFER_BIT;
        if (command->ClearDepth) { glClearDepth(1.0f); clearBit |= GL_DEPTH_BUFFER_BIT; }
        if (command->ClearStencil) { glClearStencil(0x00); clearBit |= GL_STENCIL_BUFFER_BIT; }
        glClear(clearBit);
    }
}
void Pitaya::Render::Renderer::ExecuteCommand(const Pitaya::Render::InstancedDrawCommand* command) const
{
    if (!command || command->InstanceCount == 0) { return; }

    if (command->CullFace)
    {
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glFrontFace(GL_CCW);
    }
    else
    {
        glDisable(GL_CULL_FACE);
    }

    if (command->DepthTest)
    {
        glEnable(GL_DEPTH_TEST);
    }
    else
    {
        glDisable(GL_DEPTH_TEST);
    }

    if (command->Blend)
    {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }
    else
    {
        glDisable(GL_BLEND);
    }

    Pitaya::GPU::Shader shader;
    if (Pitaya::GPU::GetShader(Pitaya::Core::PassKey<Renderer>(), command->ShaderHandle, shader))
    {
        glUseProgram(shader.Id);
    }

    Pitaya::GPU::VertexArray vaterxArray;
    if (Pitaya::GPU::GetVertexArray(Pitaya::Core::PassKey<Renderer>(), command->VertexArrayHandle, vaterxArray))
    {
        glBindVertexArray(vaterxArray.Id);

        //计算EBO偏移
        void* indicesOffset = (void*)(uintptr_t)(command->BaseIndex * sizeof(uint32_t));
        glDrawElementsInstancedBaseVertexBaseInstance(
            GL_TRIANGLES,           // mode
            command->IndexCount,    // count (每个物体的索引数)
            GL_UNSIGNED_INT,        // type
            indicesOffset,          // indices (EBO 偏移)
            command->InstanceCount, // instancecount (这次画多少个)
            command->BaseVertex,    // basevertex (VBO 偏移)
            command->BaseInstance); // baseinstance (矩阵数组在 SSBO 里的起始位置)
    }
}
void Pitaya::Render::Renderer::ExecuteCommand(const Pitaya::Render::PostProcessCommand* command) const
{
    if (!command) { return; }

    // 更新后处理 Uniform 参数
    if (command->PostProcessStep.Size > 0)
    {
        Pitaya::GPU::UniformBuffer ubo;
        if (Pitaya::GPU::GetUniformBuffer(Pitaya::Core::PassKey<Renderer>(), renderKit.PostProcessParamsUBO.Handle, ubo))
        {
            glNamedBufferSubData(ubo.Id, 0, command->PostProcessStep.Size, &command->PostProcessStep.ShaderParams);
        }
    }

    // 执行 MSAA Resolve 从多采样 FBO 解析到普通纹理 FBO
    if (command->ResolveMSAA)
    {
        Pitaya::GPU::FrameBuffer resolveFbo;
        if (Pitaya::GPU::GetFrameBuffer(Pitaya::Core::PassKey<Renderer>(), command->ResolveFrameBufferHandle, resolveFbo))
        {
            //读：MSAA FBO
            glBindFramebuffer(GL_READ_FRAMEBUFFER, resolveFbo.Id);
            //写：普通 FBO (Intermediate)
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, resolveFbo.InternalId);

            // 执行 Blit (拷贝并合并像素)
            glBlitFramebuffer(
                0, 0, command->ResolveSize.x, command->ResolveSize.y,
                0, 0, command->ResolveSize.x, command->ResolveSize.y,
                GL_COLOR_BUFFER_BIT,
                GL_NEAREST);
        }
    }

    // 执行后处理着色器渲染
    Pitaya::GPU::Shader shader;
    Pitaya::GPU::FrameBuffer readFbo;
    Pitaya::GPU::FrameBuffer writeFbo;
    if (Pitaya::GPU::GetShader(Pitaya::Core::PassKey<Renderer>(), command->ProcessShaderHandle, shader) &&
        Pitaya::GPU::GetFrameBuffer(Pitaya::Core::PassKey<Renderer>(), command->ReadFrameBufferHandle, readFbo) &&
        Pitaya::GPU::GetFrameBuffer(Pitaya::Core::PassKey<Renderer>(), command->WriteFrameBufferHandle, writeFbo))
    {
        // 绑定写入目标 
        glBindFramebuffer(GL_FRAMEBUFFER, writeFbo.Id);

        // 设置后处理状态
        glDisable(GL_DEPTH_TEST);
        glDepthMask(GL_FALSE);
        glDisable(GL_CULL_FACE);
        glDisable(GL_BLEND);

        glUseProgram(shader.Id);

        GLuint textureToRead = (readFbo.Specification.Samples > 1) ?
            readFbo.InternalColorAttachmentId :
            readFbo.ColorAttachmentId;
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureToRead);

        // 绘制全屏三角形
        Pitaya::GPU::VertexArray emptyVao;
        if (Pitaya::GPU::GetVertexArray(Pitaya::Core::PassKey<Renderer>(), renderKit.Specific.EmptyVAOHandle, emptyVao))
        {
            glBindVertexArray(emptyVao.Id);
            glDrawArrays(GL_TRIANGLES, 0, 3);
        }

        // 恢复状态
        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_TRUE);
        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
}
void Pitaya::Render::Renderer::ExecuteCommand(const Pitaya::Render::BlitToScreenCommand* command) const
{
    if (!command) { return; }

    glBindFramebuffer(GL_FRAMEBUFFER, 0); // 绑定回屏幕默认缓冲
    glViewport(0, 0, command->Size.x, command->Size.y);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    Pitaya::GPU::Shader blitShader;
    Pitaya::GPU::FrameBuffer finalFbo;
    Pitaya::GPU::VertexArray vao;
    if (Pitaya::GPU::GetShader(Pitaya::Core::PassKey<Renderer>(), renderKit.PostProcessShader.BlitShaderHandle, blitShader) &&
        Pitaya::GPU::GetFrameBuffer(Pitaya::Core::PassKey<Renderer>(), renderKit.MainDisplayRenderTarget.FinalFrameBufferHandle, finalFbo) &&
        Pitaya::GPU::GetVertexArray(Pitaya::Core::PassKey<Renderer>(), renderKit.Specific.EmptyVAOHandle, vao))
    {
        glUseProgram(blitShader.Id);
        glActiveTexture(GL_TEXTURE0);

        // 最终渲染出的 FBO 总是采样 ColorAttachmentId 因为 Final 为非 MSAA 
        glBindTexture(GL_TEXTURE_2D, finalFbo.ColorAttachmentId);

        glBindVertexArray(vao.Id);
        glDrawArrays(GL_TRIANGLES, 0, 3);
    }
}

bool Pitaya::Render::Renderer::Bake(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey, const Pitaya::Render::IBLBakeInput& input) const
{
    Pitaya::GPU::VertexArray emptyVAO;
    Pitaya::GPU::Shader equirectShader, irradianceShader, prefilterShader;
    Pitaya::GPU::FrameBuffer bakeFbo;
    Pitaya::GPU::Texture2D equirectTex;
    Pitaya::GPU::TextureCubemap envCubemap, irradianceCubemap, prefilteredCubemap;
    if (!Pitaya::GPU::GetShader(passkey, renderKit.IBL.EquirectToCubemapShaderHandle, equirectShader) ||
        !Pitaya::GPU::GetShader(passkey, renderKit.IBL.IrradianceShaderHandle, irradianceShader) ||
        !Pitaya::GPU::GetShader(passkey, renderKit.IBL.PrefilterShaderHandle, prefilterShader) ||
        !Pitaya::GPU::GetFrameBuffer(passkey, renderKit.IBL.BakeFBOHandle, bakeFbo) ||
        !Pitaya::GPU::GetTexture2D(passkey, input.Equirect, equirectTex) ||
        !Pitaya::GPU::GetTextureCubemap(passkey, input.EnvCubemap, envCubemap) ||
        !Pitaya::GPU::GetTextureCubemap(passkey, input.Irradiance, irradianceCubemap) ||
        !Pitaya::GPU::GetTextureCubemap(passkey, input.Prefiltered, prefilteredCubemap) || 
        !Pitaya::GPU::GetVertexArray(passkey, renderKit.Specific.EmptyVAOHandle, emptyVAO))
    { return false; }

    glBindVertexArray(emptyVAO.Id);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_STENCIL_TEST);
    glDisable(GL_CULL_FACE);

    glBindFramebuffer(GL_FRAMEBUFFER, bakeFbo.Id);
    // Equirect -> Cubemap (6 pass, 512x512)
    {
        glUseProgram(equirectShader.Id);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, equirectTex.Id);
        glViewport(0, 0, 512, 512);

        for (uint32_t face = 0; face < 6; ++face)
        {
            glUniform1i(0, static_cast<int>(face));
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, envCubemap.Id, 0);
            glDrawArrays(GL_TRIANGLES, 0, 3);
        }

        // GGX Prefilter 需要按 PDF 自适应采样 envCubemap 不同 mip 才能压住 firefly
        glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap.Id);
        glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
    }

    // Irradiance Convolution (6 pass, 32x32)
    {
        glUseProgram(irradianceShader.Id);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap.Id);
        glViewport(0, 0, 32, 32);

        for (uint32_t face = 0; face < 6; ++face)
        {
            glUniform1i(0, static_cast<int>(face));
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, irradianceCubemap.Id, 0);
            glDrawArrays(GL_TRIANGLES, 0, 3);
        }
    }

    // GGX Prefilter (6 face x 6 mip = 36 pass, 256x256)
    {
        glUseProgram(prefilterShader.Id);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap.Id);

        uint32_t maxMip = 6;
        for (uint32_t mip = 0; mip < maxMip; ++mip)
        {
            uint32_t mipSize = 256 >> mip;
            glViewport(0, 0, mipSize, mipSize);

            float roughness = static_cast<float>(mip) / static_cast<float>(maxMip - 1);
            glUniform1f(1, roughness);

            for (uint32_t face = 0; face < 6; ++face)
            {
                glUniform1i(0, static_cast<int>(face));
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                    GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, prefilteredCubemap.Id, mip);
                glDrawArrays(GL_TRIANGLES, 0, 3);
            }
        }
    }

    glEnable(GL_CULL_FACE);
    glEnable(GL_STENCIL_TEST);
    glEnable(GL_DEPTH_TEST);
    return true;
}
bool Pitaya::Render::Renderer::Bake(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey, const Pitaya::Render::BRDFLUTBakeInput& input) const
{
    Pitaya::GPU::VertexArray emptyVAO;
    Pitaya::GPU::Shader shader;
    Pitaya::GPU::FrameBuffer fbo;
    Pitaya::GPU::Texture2D lutTex;
    if (!Pitaya::GPU::GetShader(passkey, renderKit.IBL.BRDFLUTGenShaderHandle, shader) ||
        !Pitaya::GPU::GetFrameBuffer(passkey, renderKit.IBL.BakeFBOHandle, fbo) ||
        !Pitaya::GPU::GetTexture2D(passkey, input.Output, lutTex) || 
        !Pitaya::GPU::GetVertexArray(passkey, renderKit.Specific.EmptyVAOHandle, emptyVAO))
    { return false; }

    glBindVertexArray(emptyVAO.Id);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_STENCIL_TEST);
    glDisable(GL_CULL_FACE);

    glBindFramebuffer(GL_FRAMEBUFFER, fbo.Id);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, lutTex.Id, 0);
    glUseProgram(shader.Id);
    glViewport(0, 0, static_cast<GLsizei>(input.Resolution), static_cast<GLsizei>(input.Resolution));
    glDrawArrays(GL_TRIANGLES, 0, 3);

    glEnable(GL_CULL_FACE);
    glEnable(GL_STENCIL_TEST);
    glEnable(GL_DEPTH_TEST);
    return true;
}
#endif
