#include<Render/Frontend/Renderer.h>
#include<GPU/Common/BindPoint.h>
#include<GPU/Frontend/Buffer/ShaderStorageBuffer.h>
#include<GPU/Frontend/Buffer/UniformBuffer.h>
#include<GPU/Frontend/Texture/Texture2D.h>
#include<GPU/Frontend/Texture/Texture2DArray.h>
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
	//glEnable(GL_FRAMEBUFFER_SRGB);			//开启SRGB帧缓冲区进行Gamma矫正（通过后处理进行矫正）

	backendStorage.Cast<OpenGLGraphicsContext>().Window = glfwWindow;
	return true;
}
void Pitaya::Render::Renderer::ReleaseRenderContext()
{
	backendStorage.Cast<OpenGLGraphicsContext>().Window = nullptr;
}

void Pitaya::Render::Renderer::SwapBuffer() const
{
	glfwSwapBuffers(backendStorage.Cast<OpenGLGraphicsContext>().Window);
}
void Pitaya::Render::Renderer::NewRenderFrame()
{
	Pitaya::GPU::ShaderStorageBuffer shaderStorageBuffer;

	// 动态扩容/上传 Transform SSBO
	size_t uploadTransformCount = renderPacket.back.InstanceModelTransforms.size();
	if (uploadTransformCount > 0)
	{
		if (Pitaya::GPU::GetShaderStorageBuffer(Pitaya::Core::PassKey<Renderer>(), globalRHI.InstanceModelTransformSSBO.Handle, shaderStorageBuffer))
		{
			size_t requiredSize = uploadTransformCount * sizeof(InstanceTransformInfo);
			glBindBufferBase(GL_SHADER_STORAGE_BUFFER,
				static_cast<uint32_t>(Pitaya::GPU::SSBOBindPoint::InstanceModelTransform),
				shaderStorageBuffer.Id);

			// 检查预申请的显存能不能装下当前帧的数据
			if (requiredSize > globalRHI.InstanceModelTransformSSBO.Capacity)
			{
				// 1.5 倍扩容策略
				globalRHI.InstanceModelTransformSSBO.Capacity = requiredSize + (requiredSize / 2);
				glNamedBufferData(shaderStorageBuffer.Id, globalRHI.InstanceModelTransformSSBO.Capacity, nullptr, GL_DYNAMIC_DRAW);	// 使用 glNamedBufferData 重新分配显存块
			}

			// 将最新推算好的数据安全更新到 SSBO 中
			glNamedBufferSubData(shaderStorageBuffer.Id, 0, requiredSize, renderPacket.back.InstanceModelTransforms.data());
		}
	}

	// 动态扩容/上传 Bone SSBO
	size_t uploadBoneCount = renderPacket.back.BoneMatrices.size();
	if (uploadBoneCount > 0)
	{
		if (Pitaya::GPU::GetShaderStorageBuffer(Pitaya::Core::PassKey<Renderer>(), globalRHI.BoneInverseMatriceSSBO.Handle, shaderStorageBuffer))
		{
			size_t requiredBoneSize = uploadBoneCount * sizeof(glm::mat4);
			glBindBufferBase(GL_SHADER_STORAGE_BUFFER,
				static_cast<uint32_t>(Pitaya::GPU::SSBOBindPoint::BoneInverseMatrice),
				shaderStorageBuffer.Id);

			if (requiredBoneSize > globalRHI.BoneInverseMatriceSSBO.Capacity)
			{
				// 1.5倍扩容
				globalRHI.BoneInverseMatriceSSBO.Capacity = requiredBoneSize + (requiredBoneSize / 2);
				glNamedBufferData(shaderStorageBuffer.Id, globalRHI.BoneInverseMatriceSSBO.Capacity, nullptr, GL_DYNAMIC_DRAW);
			}

			glNamedBufferSubData(shaderStorageBuffer.Id, 0, requiredBoneSize, renderPacket.back.BoneMatrices.data());
		}
	}

	// 动态扩容 Light SSBO（仅扩容，不在此处上传数据，由 ExecuteCommand(BeginPassCommand) 每 Pass 上传）
	size_t totalLightCount = renderPacket.back.Lights.size();
	size_t headerSize = 4 * sizeof(uint32_t);
	size_t maxRequiredSize = headerSize + totalLightCount * sizeof(Pitaya::Render::LightInfo);
	if (Pitaya::GPU::GetShaderStorageBuffer(Pitaya::Core::PassKey<Renderer>(), globalRHI.SceneLightsSSBO.Handle, shaderStorageBuffer))
	{
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER,
			static_cast<uint32_t>(Pitaya::GPU::SSBOBindPoint::SceneLights),
			shaderStorageBuffer.Id);

		if (maxRequiredSize > globalRHI.SceneLightsSSBO.Capacity)
		{
			globalRHI.SceneLightsSSBO.Capacity = maxRequiredSize + (maxRequiredSize / 2);
			glNamedBufferData(shaderStorageBuffer.Id, globalRHI.SceneLightsSSBO.Capacity, nullptr, GL_DYNAMIC_DRAW);
		}
	}

	// 动态扩容/上传 Shadow Atlas
	auto EnsureAtlas = [](GlobalRHI::ShadowAtlas& atlas, uint32_t requiredLayers, uint32_t resolution)
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
				static_cast<int>(newCapacity), true);
			atlas.LayerCapacity = newCapacity;
		};
	EnsureAtlas(globalRHI.CSMAtlas, renderPacket.back.RequiredCSMLayers, GlobalRHI::ShadowAtlas::CSMResolution);
	EnsureAtlas(globalRHI.SpotShadowAtlas, renderPacket.back.RequiredSpotLayers, GlobalRHI::ShadowAtlas::SpotResolution);
	EnsureAtlas(globalRHI.PointShadowAtlas, renderPacket.back.RequiredPointLayers, GlobalRHI::ShadowAtlas::PointResolution);
	if (!renderPacket.back.ShadowSSBOData.empty())
	{
		if (Pitaya::GPU::GetShaderStorageBuffer(Pitaya::Core::PassKey<Renderer>(),
			globalRHI.ShadowSSBO.Handle, shaderStorageBuffer))
		{
			size_t requiredSize = renderPacket.back.ShadowSSBOData.size();

			glBindBufferBase(GL_SHADER_STORAGE_BUFFER,
				static_cast<uint32_t>(Pitaya::GPU::SSBOBindPoint::Shadow),
				shaderStorageBuffer.Id);

			if (requiredSize > globalRHI.ShadowSSBO.Capacity)
			{
				globalRHI.ShadowSSBO.Capacity = requiredSize + (requiredSize / 2);
				glNamedBufferData(shaderStorageBuffer.Id,
					static_cast<GLsizeiptr>(globalRHI.ShadowSSBO.Capacity),
					nullptr, GL_DYNAMIC_DRAW);
			}

			glNamedBufferSubData(shaderStorageBuffer.Id, 0,
				static_cast<GLsizeiptr>(requiredSize),
				renderPacket.back.ShadowSSBOData.data());
		}
	}
	auto BindShadowAtlas = [](const GlobalRHI::ShadowAtlas& atlas, uint32_t unit)
		{
			if (atlas.LayerCapacity == 0) { return; }
			Pitaya::GPU::Texture2DArray tex;
			if (Pitaya::GPU::GetTexture2DArray(Pitaya::Core::PassKey<Pitaya::Render::Renderer>(), atlas.TextureHandle, tex))
			{
				glActiveTexture(GL_TEXTURE0 + unit);
				glBindTexture(GL_TEXTURE_2D_ARRAY, tex.Id);
			}
		};
	BindShadowAtlas(globalRHI.CSMAtlas, static_cast<uint32_t>(Pitaya::GPU::TextureSlot::CSM));
	BindShadowAtlas(globalRHI.SpotShadowAtlas, static_cast<uint32_t>(Pitaya::GPU::TextureSlot::SPOT));
	BindShadowAtlas(globalRHI.PointShadowAtlas, static_cast<uint32_t>(Pitaya::GPU::TextureSlot::POINT));
}

void Pitaya::Render::Renderer::ExecuteCommand(const Pitaya::Render::BeginShadowPassCommand* command) const
{
	if (!command) return;

	// 选择 atlas
	Pitaya::Core::SlotMap<Pitaya::GPU::Texture2DArray>::Handle atlasTextureHandle;
	uint32_t LayerCapacity = 0;
	switch (command->LightType)
	{
	case 0:
		atlasTextureHandle = globalRHI.CSMAtlas.TextureHandle;
		LayerCapacity = globalRHI.CSMAtlas.LayerCapacity;
		break;

	case 1:
		atlasTextureHandle = globalRHI.PointShadowAtlas.TextureHandle;
		LayerCapacity = globalRHI.PointShadowAtlas.LayerCapacity;
		break;

	case 2:
		atlasTextureHandle = globalRHI.SpotShadowAtlas.TextureHandle;
		LayerCapacity = globalRHI.SpotShadowAtlas.LayerCapacity;
		break;

	default: return;
	}
	if (LayerCapacity == 0) { return; }

	// 通过 Handle 获取 Texture2DArray 和 Shadow FBO
	Pitaya::GPU::Texture2DArray texArray;
	Pitaya::GPU::FrameBuffer shadowFbo;
	if (Pitaya::GPU::GetTexture2DArray(Pitaya::Core::PassKey<Pitaya::Render::Renderer>(), atlasTextureHandle, texArray) &&
		Pitaya::GPU::GetFrameBuffer(Pitaya::Core::PassKey<Pitaya::Render::Renderer>(), globalRHI.Specific.ShadowFBO, shadowFbo))
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
		if (Pitaya::GPU::GetUniformBuffer(Pitaya::Core::PassKey<Pitaya::Render::Renderer>(), globalRHI.CameraSnapshotUBO.Handle, cameraUbo))
		{
			glNamedBufferSubData(cameraUbo.Id, 0, sizeof(ShadowCameraBlock), &camBlock);
		}
	}
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
	if (Pitaya::GPU::GetUniformBuffer(Pitaya::Core::PassKey<Renderer>(), globalRHI.CameraSnapshotUBO.Handle, cameraUbo))
	{
		glNamedBufferSubData(cameraUbo.Id, 0, sizeof(Pitaya::Core::CameraSnapshot), &command->CameraSnapshot);
	}

	// 每 Pass 刷新光源 SSBO
	Pitaya::GPU::ShaderStorageBuffer lightsSSBO;
	if (Pitaya::GPU::GetShaderStorageBuffer(Pitaya::Core::PassKey<Renderer>(), globalRHI.SceneLightsSSBO.Handle, lightsSSBO))
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

	Pitaya::GPU::Texture2D texture2D;
	for (size_t i = 0; i < Pitaya::GPU::MaterialTextureSlotCount; i++)
	{
		if (Pitaya::GPU::GetTexture2D(Pitaya::Core::PassKey<Renderer>(), command->TextureHandles[i], texture2D))
		{
			glActiveTexture(GL_TEXTURE0 + i);
			glBindTexture(GL_TEXTURE_2D, texture2D.Id);
		}
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
		if (Pitaya::GPU::GetUniformBuffer(Pitaya::Core::PassKey<Renderer>(), globalRHI.PostProcessParamsUBO.Handle, ubo))
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
		if (Pitaya::GPU::GetVertexArray(Pitaya::Core::PassKey<Renderer>(), globalRHI.Specific.EmptyVAOHandle, emptyVao))
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
	if (Pitaya::GPU::GetShader(Pitaya::Core::PassKey<Renderer>(), globalRHI.PostProcessShader.BlitShaderHandle, blitShader) &&
		Pitaya::GPU::GetFrameBuffer(Pitaya::Core::PassKey<Renderer>(), globalRHI.MainDisplayRenderTarget.FinalFrameBufferHandle, finalFbo) &&
		Pitaya::GPU::GetVertexArray(Pitaya::Core::PassKey<Renderer>(), globalRHI.Specific.EmptyVAOHandle, vao))
	{
		glUseProgram(blitShader.Id);
		glActiveTexture(GL_TEXTURE0);

		// 最终渲染出的 FBO 总是采样 ColorAttachmentId 因为 Final 为非 MSAA 
		glBindTexture(GL_TEXTURE_2D, finalFbo.ColorAttachmentId);

		glBindVertexArray(vao.Id);
		glDrawArrays(GL_TRIANGLES, 0, 3);
	}
}
#endif