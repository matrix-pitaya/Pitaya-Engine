#include<Render/Backend/OpenGL/OpenGLRenderer.h>
#include<GPU/Common/BindPoint.h>
#include<Log/Common/FuncTable.h>

#ifdef PITAYA_EDITOR
#include<Editor/GUI/ImGui/imgui.h>
#include<Editor/GUI/ImGui/backends/imgui_impl_opengl3.h>
#endif

#include<stdexcept>
#include<string>

bool Pitaya::Render::OpenGLRenderer::InitializeRenderContext(void* nativeWindow)
{
	glfwWindow = reinterpret_cast<GLFWwindow*>(nativeWindow);
	if (!glfwWindow)
	{
		Pitaya::Log::Error("OpenGL Window Get Fail!");
		return false;
	}

	//创建OpenGL上下文
	glfwMakeContextCurrent(glfwWindow);
	glfwSwapInterval(1);

	//初始化GLEW
	if (glewInit() != GLEW_OK) 
	{ 
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
	const Pitaya::Render::Renderer::RenderPacket::Buffer& back = renderPacket.GetBackBuffer();
	if (!back.InstanceModelTransforms.empty())
	{
		size_t dataSize = back.InstanceModelTransforms.size() * sizeof(glm::mat4);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER,
			static_cast<uint32_t>(Pitaya::GPU::SSBOBindPoint::InstanceModelTransform),
			globalRHI.InstanceModelTransformSSBO);
		glNamedBufferSubData(globalRHI.InstanceModelTransformSSBO, 0, dataSize, back.InstanceModelTransforms.data());
	}

	if (!back.BoneMatrices.empty())
	{
		size_t boneDataSize = back.BoneMatrices.size() * sizeof(glm::mat4);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER,
			static_cast<uint32_t>(Pitaya::GPU::SSBOBindPoint::BoneInverseMatrice),
			globalRHI.BoneInverseMatriceSSBO);
		glNamedBufferSubData(globalRHI.BoneInverseMatriceSSBO, 0, boneDataSize, back.BoneMatrices.data());
	}
}

void Pitaya::Render::OpenGLRenderer::ExecuteCommand(const Pitaya::Render::BeginPassCommand* command) const
{
	if (!command) { return; }

	//绑定并填充CameraUBO
	glBindBuffer(GL_UNIFORM_BUFFER, globalRHI.CameraSnapshotUBO);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(Pitaya::Core::CameraSnapshot), &command->CameraSnapshot);

	//绑定Framebuffer
	const Pitaya::Render::RenderTargetSnapshot& renderTarget = command->RenderTargetSnapshot;
	glBindFramebuffer(GL_FRAMEBUFFER, renderTarget.SceneFrameBuffer);
	glViewport(renderTarget.Rect.Position.x, renderTarget.Rect.Position.y, renderTarget.Rect.Size.x, renderTarget.Rect.Size.y);
	glClearColor(renderTarget.ClearColor.r, renderTarget.ClearColor.g, renderTarget.ClearColor.b, renderTarget.ClearColor.a);
	uint64_t clearBit = GL_COLOR_BUFFER_BIT;
	if (renderTarget.ClearDepth)
	{
		glClearDepth(1.0f);
		clearBit |= GL_DEPTH_BUFFER_BIT;
	}
	if (renderTarget.ClearStencil)
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

	if (command->PostProcessShader > 0)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, command->WriteFrameBuffer);

		glDisable(GL_DEPTH_TEST);
		glDepthMask(GL_FALSE);
		glDisable(GL_CULL_FACE);
		glDisable(GL_BLEND);

		glUseProgram(command->PostProcessShader);

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
