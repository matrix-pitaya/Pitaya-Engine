#include<Render/Backend/OpenGL/OpenGLRenderer.h>
#include<GPU/Common/BindPoint.h>
#include<Log/Common/FuncTable.h>

#include<stdexcept>
#include<string>

bool Pitaya::Render::OpenGLRenderer::InitializeRenderContext(void* nativeWindow)
{
	glfwWindow = reinterpret_cast<GLFWwindow*>(nativeWindow);
	if (!glfwWindow)
	{
		MessageBoxA(NULL, "Initialize OpenGL Context Failed! Reinterpret Cast NativeWindow Fail!", "Error", MB_OK);
		exit(-1);
		return false;
	}

	//创建OpenGL上下文
	glfwMakeContextCurrent(glfwWindow);
	glfwSwapInterval(1);

	//初始化GLEW
	if (glewInit() != GLEW_OK) 
	{ 
		MessageBoxA(NULL, "Initialize OpenGL Context Failed! GLEW InitFail!", "Error", MB_OK);
		exit(-1);
		return false; 
	}

	glEnable(GL_DEPTH_TEST);					//开启深度测试
	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_TRUE);
	glEnable(GL_CULL_FACE);						//开启面剔除
	glEnable(GL_STENCIL_TEST);					//开启模板测试
	glStencilFunc(GL_ALWAYS, 1, 0xFF);			//设置总是通过模板测试
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);	//设置更新模板缓冲区方式
	glStencilMask(0xFF);						//设置允许写入模板缓冲区
	glEnable(GL_MULTISAMPLE);					//启用多重采样抗锯齿
	//glEnable(GL_FRAMEBUFFER_SRGB);			//开启SRGB帧缓冲区进行Gamma矫正（通过后处理进行矫正）

	return true;
}
void Pitaya::Render::OpenGLRenderer::ReleaseRenderContext()
{
	glfwWindow = nullptr;
}

void Pitaya::Render::OpenGLRenderer::SwapBuffer() const
{
	glfwSwapBuffers(glfwWindow);
}
void Pitaya::Render::OpenGLRenderer::NewRenderFrame()
{
	const auto& back = renderPacket.GetBackBuffer();

	// 动态扩容/上传 Transform SSBO
	size_t uploadTransformCount = back.InstanceModelTransforms.size();
	if (uploadTransformCount > 0)
	{
		size_t requiredSize = uploadTransformCount * sizeof(InstanceTransformInfo);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER,
			static_cast<uint32_t>(Pitaya::GPU::SSBOBindPoint::InstanceModelTransform),
			globalRHI.InstanceModelTransformSSBO);

		// 检查预申请的显存能不能装下当前帧的数据
		if (requiredSize > globalRHI.TransformSSBOCapacity)
		{
			// 1.5 倍扩容策略
			globalRHI.TransformSSBOCapacity = requiredSize + (requiredSize / 2);
			glNamedBufferData(globalRHI.InstanceModelTransformSSBO, globalRHI.TransformSSBOCapacity, nullptr, GL_DYNAMIC_DRAW);	// 使用 glNamedBufferData 重新分配显存块
		}

		// 将最新推算好的数据安全更新到 SSBO 中
		glNamedBufferSubData(globalRHI.InstanceModelTransformSSBO, 0, requiredSize, back.InstanceModelTransforms.data());
	}

	// 动态扩容/上传 Bone SSBO
	size_t uploadBoneCount = back.BoneMatrices.size();
	if (uploadBoneCount > 0)
	{
		size_t requiredBoneSize = uploadBoneCount * sizeof(glm::mat4);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER,
			static_cast<uint32_t>(Pitaya::GPU::SSBOBindPoint::BoneInverseMatrice),
			globalRHI.BoneInverseMatriceSSBO);

		if (requiredBoneSize > globalRHI.BoneSSBOCapacity)
		{
			// 1.5倍扩容
			globalRHI.BoneSSBOCapacity = requiredBoneSize + (requiredBoneSize / 2);
			glNamedBufferData(globalRHI.BoneInverseMatriceSSBO, globalRHI.BoneSSBOCapacity, nullptr, GL_DYNAMIC_DRAW);
		}

		glNamedBufferSubData(globalRHI.BoneInverseMatriceSSBO, 0, requiredBoneSize, back.BoneMatrices.data());
	}

	// 动态扩容/上传 Light SSBO
	size_t uploadLightCount = back.Lights.size();
	size_t headerSize = 4 * sizeof(uint32_t);	// Header固定16字节对齐 (1个有效uint + 3个padding)
	size_t dataSize = uploadLightCount * sizeof(Pitaya::Render::LightInfo);
	size_t requiredLightSize = headerSize + dataSize;
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER,
		static_cast<uint32_t>(Pitaya::GPU::SSBOBindPoint::SceneLights),
		globalRHI.SceneLightsSSBO);
	if (requiredLightSize > globalRHI.LightSSBOCapacity)	// 判断是否需要扩容
	{
		// 1.5 倍扩容
		globalRHI.LightSSBOCapacity = requiredLightSize + (requiredLightSize / 2);
		glNamedBufferData(globalRHI.SceneLightsSSBO, globalRHI.LightSSBOCapacity, nullptr, GL_DYNAMIC_DRAW);
	}
	// 始终上传 16 字节头部信息 告诉 GPU 真实有效光源数
	uint32_t lightHeader[4] = { static_cast<uint32_t>(uploadLightCount), 0, 0, 0 };	
	glNamedBufferSubData(globalRHI.SceneLightsSSBO, 0, headerSize, lightHeader);
	// 数据头上传之后 上传真实的结构体数组数据
	if (uploadLightCount > 0)
	{
		glNamedBufferSubData(globalRHI.SceneLightsSSBO, headerSize, dataSize, back.Lights.data());
	}
}

void Pitaya::Render::OpenGLRenderer::ExecuteCommand(const Pitaya::Render::BeginPassCommand* command) const
{
	if (!command) { return; }

	//绑定并填充CameraUBO
	glBindBuffer(GL_UNIFORM_BUFFER, globalRHI.CameraSnapshotUBO);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(Pitaya::Core::CameraSnapshot), &command->CameraSnapshot);

	//绑定Framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, command->SceneFrameBuffer);
	glViewport(command->Rect.Position.x, command->Rect.Position.y, command->Rect.Size.x, command->Rect.Size.y);
	glClearColor(command->ClearColor.r, command->ClearColor.g, command->ClearColor.b, command->ClearColor.a);
	uint64_t clearBit = GL_COLOR_BUFFER_BIT;
	if (command->ClearDepth)
	{
		glClearDepth(1.0f);
		clearBit |= GL_DEPTH_BUFFER_BIT;
	}
	if (command->ClearStencil)
	{
		glClearStencil(0x00);
		clearBit |= GL_STENCIL_BUFFER_BIT;
	}
	glClear(clearBit);
}
void Pitaya::Render::OpenGLRenderer::ExecuteCommand(const Pitaya::Render::InstancedDrawCommand* command) const
{
	if (!command || command->InstanceCount == 0) { return; }

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

	glUseProgram(command->Shader);

	//绑定纹理
	for (size_t i = 0; i < static_cast<size_t>(Pitaya::GPU::TextureUsage::Unknown); i++)
	{
		uint32_t textureID = command->Textures[i];
		if (textureID > 0)
		{
			glActiveTexture(GL_TEXTURE0 + i);
			glBindTexture(GL_TEXTURE_2D, textureID);
		}
	}

	//TODO 处理自定义材质属性

	glBindVertexArray(command->VertexArray);

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
void Pitaya::Render::OpenGLRenderer::ExecuteCommand(const Pitaya::Render::PostProcessCommand* command) const
{
	if (!command) { return; }

	if (command->PostProcessStep.Size > 0)
	{
		glBindBuffer(GL_UNIFORM_BUFFER, globalRHI.PostProcessUBO);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, command->PostProcessStep.Size, &command->PostProcessStep.ShaderParams);
	}
	
	//如果是多重采样 解析Resolve 到普通纹理
	if (command->ResolveMSAA)
	{
		//读：MSAA FBO
		glBindFramebuffer(GL_READ_FRAMEBUFFER, command->ResolveReadFrameBuffer);
		//写：普通 FBO (Intermediate)
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, command->ResolveWriteFrameBuffer);

		// 执行 Blit (拷贝并合并像素)
		glBlitFramebuffer(
			0, 0, command->ResolveSize.x, command->ResolveSize.y, // Source Rect
			0, 0, command->ResolveSize.x, command->ResolveSize.y, // Dest Rect
			GL_COLOR_BUFFER_BIT,
			GL_NEAREST); // 由于尺寸完全一致，NEAREST 即可
	}

	if (command->ProcessShader > 0)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, command->WriteFrameBuffer);

		glDisable(GL_DEPTH_TEST);
		glDepthMask(GL_FALSE);
		glDisable(GL_CULL_FACE);
		glDisable(GL_BLEND);

		glUseProgram(command->ProcessShader);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, command->ReadTexture);

		glBindVertexArray(globalRHI.EmptyVAO);
		glDrawArrays(GL_TRIANGLES, 0, 3);

		glEnable(GL_DEPTH_TEST);
		glDepthMask(GL_TRUE);
		glBindVertexArray(0);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}
void Pitaya::Render::OpenGLRenderer::ExecuteCommand(const Pitaya::Render::BlitToScreenCommand* command) const
{
	if (!command) { return; }
	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, command->Size.x, command->Size.y);
	glClearColor(Pitaya::Core::Color::Dark.r, Pitaya::Core::Color::Dark.g, Pitaya::Core::Color::Dark.b, Pitaya::Core::Color::Dark.a);
	glClearDepth(1.0f);
	glClearStencil(0x00);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	glUseProgram(globalRHI.BlitShader);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, globalRHI.MainFinalColorAttachment);

	glBindVertexArray(globalRHI.EmptyVAO);
	glDrawArrays(GL_TRIANGLES, 0, 3);

	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}
