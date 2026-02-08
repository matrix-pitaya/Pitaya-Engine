#include<Renderer/Backend/OpenGL/OpenGLRenderer.h>

#include<Renderer/Backend/OpenGL/Vertex/OpenGLVertexArray.h>
#include<Renderer/Backend/OpenGL/Vertex/OpenGLVertexBuffer.h>
#include<Renderer/Backend/OpenGL/Vertex/OpenGLElementBuffer.h>

#include<Engine/Internal/Thread/Thread.h>
#include<Engine/API/Log/Log.h>

#include<stdexcept>
#include<string>

bool Pitaya::Engine::Renderer::OpenGLRenderer::Initialize()
{
	if (!window)
	{
		std::runtime_error("Renderer module missing!");
		return false;
	}

	renderThreadToken = Pitaya::Engine::Thread::GetThreadModel()->RegisterThread("Render", &Pitaya::Engine::Renderer::OpenGLRenderer::RenderThread, this);
	return true;
}
void Pitaya::Engine::Renderer::OpenGLRenderer::Release()
{
	isRunning = false;
	cond.notify_one();
	Pitaya::Engine::Thread::GetThreadModel()->UnregisterThread(renderThreadToken);
	glfwWindow = nullptr;
}
void Pitaya::Engine::Renderer::OpenGLRenderer::SwapBuffer() const
{
	glfwSwapBuffers(glfwWindow);
}
bool Pitaya::Engine::Renderer::OpenGLRenderer::InitOpenGLContext()
{
	glfwWindow = reinterpret_cast<GLFWwindow*>(window->GetNativeWindow());
	if (!glfwWindow)
	{
		Pitaya::Engine::Log::LogError("OpenGL Window Get Fail!");
		return false;
	}

	//创建OpenGL上下文
	glfwMakeContextCurrent(glfwWindow);

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
	glEnable(GL_FRAMEBUFFER_SRGB);				//开启SRGB帧缓冲区进行Gamma矫正

	isRunning = true;
	return true;
}
void Pitaya::Engine::Renderer::OpenGLRenderer::RenderThread()
{
	if (!InitOpenGLContext())
	{
		throw std::runtime_error("Init OpenGL Context Fail!");
	}

	while (true)
	{
		std::unique_lock<std::mutex> lock(mutex);
		cond.wait(lock, [this] { return !back.empty() || !isRunning; });

		ParseCommand();
		if (back.empty() && !isRunning)
		{
			break;
		}
	}
}
void Pitaya::Engine::Renderer::OpenGLRenderer::BeginRenderFrame()
{
	//清空失效的渲染命令
	front.clear();
	front.reserve(64 * 1024);
}
void Pitaya::Engine::Renderer::OpenGLRenderer::EndRenderFrame()
{
	//结束当前渲染帧 提交交换buffer的命令
	PushCommandToFrontBuffer(Pitaya::Engine::Renderer::RenderCommandType::SwapBuffer, SwapBufferCommand());
	
	//交换渲染缓冲区
	{
		std::lock_guard<std::mutex> lock(mutex);
		std::swap(front, back);
	}

	//唤醒渲染线程工作
	cond.notify_one();
}
void Pitaya::Engine::Renderer::OpenGLRenderer::BeginPass(const Pitaya::Engine::Renderer::CameraSnapshot& cameraSnapshot)
{
	BeginPassCommand beginPassCommand;
	beginPassCommand.ClearColor = glm::vec4(0.1f, 0.2f, 0.3f, 1.0f);
	beginPassCommand.ClearDepth = true;
	beginPassCommand.ClearStencil = true;
	PushCommandToFrontBuffer(Pitaya::Engine::Renderer::RenderCommandType::BeginPass, beginPassCommand);
}
void Pitaya::Engine::Renderer::OpenGLRenderer::EndPass()
{
	std::sort(pass.begin(), pass.end(),
		[](const DrawCommand& a, const DrawCommand& b) {return a.SortKey > b.SortKey; });
	for (auto& cmmand : pass)
	{
		PushCommandToFrontBuffer(Pitaya::Engine::Renderer::RenderCommandType::Draw, cmmand);
	}
	pass.clear();
}
void Pitaya::Engine::Renderer::OpenGLRenderer::Submit()
{

}
Pitaya::Engine::Renderer::VertexArray* Pitaya::Engine::Renderer::OpenGLRenderer::CreateVertexArray()
{
	return new OpenGLVertexArray();
}
Pitaya::Engine::Renderer::VertexBuffer* Pitaya::Engine::Renderer::OpenGLRenderer::CreateVertexBuffer(float* vertices, uint32_t size)
{
	return new OpenGLVertexBuffer(vertices, size);
}
Pitaya::Engine::Renderer::ElementBuffer* Pitaya::Engine::Renderer::OpenGLRenderer::CreateElementBuffer(uint32_t* indices, uint32_t count)
{
	return new OpenGLElementBuffer(indices, count);
}