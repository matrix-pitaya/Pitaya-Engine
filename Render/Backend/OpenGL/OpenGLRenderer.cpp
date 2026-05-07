#include<Render/Frontend/Renderer.h>
#include<GPU/Common/BindPoint.h>
#include<GPU/Frontend/Buffer/ShaderStorageBuffer.h>
#include<GPU/Frontend/Buffer/UniformBuffer.h>
#include<GPU/Frontend/Texture/Texture2D.h>
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
		if (Pitaya::GPU::GetShaderStorageBuffer(Pitaya::Core::PassKey<Renderer>(), globalRHI.InstanceModelTransformSSBOHandle, shaderStorageBuffer))
		{
			size_t requiredSize = uploadTransformCount * sizeof(InstanceTransformInfo);
			glBindBufferBase(GL_SHADER_STORAGE_BUFFER,
				static_cast<uint32_t>(Pitaya::GPU::SSBOBindPoint::InstanceModelTransform),
				shaderStorageBuffer.Id);

			// 检查预申请的显存能不能装下当前帧的数据
			if (requiredSize > globalRHI.TransformSSBOCapacity)
			{
				// 1.5 倍扩容策略
				globalRHI.TransformSSBOCapacity = requiredSize + (requiredSize / 2);
				glNamedBufferData(shaderStorageBuffer.Id, globalRHI.TransformSSBOCapacity, nullptr, GL_DYNAMIC_DRAW);	// 使用 glNamedBufferData 重新分配显存块
			}

			// 将最新推算好的数据安全更新到 SSBO 中
			glNamedBufferSubData(shaderStorageBuffer.Id, 0, requiredSize, renderPacket.back.InstanceModelTransforms.data());
		}
	}

	// 动态扩容/上传 Bone SSBO
	size_t uploadBoneCount = renderPacket.back.BoneMatrices.size();
	if (uploadBoneCount > 0)
	{
		if (Pitaya::GPU::GetShaderStorageBuffer(Pitaya::Core::PassKey<Renderer>(), globalRHI.BoneInverseMatriceSSBOHandle, shaderStorageBuffer))
		{
			size_t requiredBoneSize = uploadBoneCount * sizeof(glm::mat4);
			glBindBufferBase(GL_SHADER_STORAGE_BUFFER,
				static_cast<uint32_t>(Pitaya::GPU::SSBOBindPoint::BoneInverseMatrice),
				shaderStorageBuffer.Id);

			if (requiredBoneSize > globalRHI.BoneSSBOCapacity)
			{
				// 1.5倍扩容
				globalRHI.BoneSSBOCapacity = requiredBoneSize + (requiredBoneSize / 2);
				glNamedBufferData(shaderStorageBuffer.Id, globalRHI.BoneSSBOCapacity, nullptr, GL_DYNAMIC_DRAW);
			}

			glNamedBufferSubData(shaderStorageBuffer.Id, 0, requiredBoneSize, renderPacket.back.BoneMatrices.data());
		}
	}

	// 动态扩容/上传 Light SSBO
	if (Pitaya::GPU::GetShaderStorageBuffer(Pitaya::Core::PassKey<Renderer>(), globalRHI.SceneLightsSSBOHandle, shaderStorageBuffer))
	{
		size_t uploadLightCount = renderPacket.back.Lights.size();
		size_t headerSize = 4 * sizeof(uint32_t);	// Header固定16字节对齐 (1个有效uint + 3个padding)
		size_t dataSize = uploadLightCount * sizeof(Pitaya::Render::LightInfo);
		size_t requiredLightSize = headerSize + dataSize;
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER,
			static_cast<uint32_t>(Pitaya::GPU::SSBOBindPoint::SceneLights),
			shaderStorageBuffer.Id);
		if (requiredLightSize > globalRHI.LightSSBOCapacity)	// 判断是否需要扩容
		{
			// 1.5 倍扩容
			globalRHI.LightSSBOCapacity = requiredLightSize + (requiredLightSize / 2);
			glNamedBufferData(shaderStorageBuffer.Id, globalRHI.LightSSBOCapacity, nullptr, GL_DYNAMIC_DRAW);
		}
		// 始终上传 16 字节头部信息 告诉 GPU 真实有效光源数
		uint32_t lightHeader[4] = { static_cast<uint32_t>(uploadLightCount), 0, 0, 0 };
		glNamedBufferSubData(shaderStorageBuffer.Id, 0, headerSize, lightHeader);
		// 数据头上传之后 上传真实的结构体数组数据
		if (uploadLightCount > 0)
		{
			glNamedBufferSubData(shaderStorageBuffer.Id, headerSize, dataSize, renderPacket.back.Lights.data());
		}
	}
}

void Pitaya::Render::Renderer::ExecuteCommand(const Pitaya::Render::BeginPassCommand* command) const
{
	if (!command) { return; }

	Pitaya::GPU::UniformBuffer cameraUbo;
	if (Pitaya::GPU::GetUniformBuffer(Pitaya::Core::PassKey<Renderer>(), globalRHI.CameraSnapshotUBOHandle, cameraUbo))
	{
		glNamedBufferSubData(cameraUbo.Id, 0, sizeof(Pitaya::Core::CameraSnapshot), &command->CameraSnapshot);
	}

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
		if (Pitaya::GPU::GetUniformBuffer(Pitaya::Core::PassKey<Renderer>(), globalRHI.PostProcessUBOHandle, ubo))
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
		if (Pitaya::GPU::GetVertexArray(Pitaya::Core::PassKey<Renderer>(), globalRHI.EmptyVAOHandle, emptyVao))
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
	if (Pitaya::GPU::GetShader(Pitaya::Core::PassKey<Renderer>(), globalRHI.BlitShaderHandle, blitShader) &&
		Pitaya::GPU::GetFrameBuffer(Pitaya::Core::PassKey<Renderer>(), globalRHI.MainFinalFrameBufferHandle, finalFbo) &&
		Pitaya::GPU::GetVertexArray(Pitaya::Core::PassKey<Renderer>(), globalRHI.EmptyVAOHandle, vao))
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