#include<Renderer/Backend/OpenGL/OpenGLRenderer.h>

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
}
bool Pitaya::Engine::Renderer::OpenGLRenderer::InitOpenGLContext()
{
	openGLWindow = reinterpret_cast<GLFWwindow*>(window->GetNativeWindow());
	if (!openGLWindow)
	{
		Pitaya::Engine::Log::LogError("OpenGL Window Get Fail!");
		return false;
	}

	//创建OpenGL上下文
	glfwMakeContextCurrent(openGLWindow);

	//初始化GLEW
	if (glewInit() != GLEW_OK)
	{
		glfwDestroyWindow(openGLWindow); //销毁窗口
		glfwTerminate(); //卸载GLFW库
		return false;
	}

	glEnable(GL_DEPTH_TEST);	//开启深度测试
	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_TRUE);
	glEnable(GL_CULL_FACE);		//开启面剔除
	glEnable(GL_STENCIL_TEST);	//开启模板测试
	glStencilFunc(GL_ALWAYS, 1, 0xFF);			//设置总是通过模板测试
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);	//设置更新模板缓冲区方式
	glStencilMask(0xFF);						//设置允许写入模板缓冲区
	glEnable(GL_MULTISAMPLE);					//启用多重采样抗锯齿
	glEnable(GL_FRAMEBUFFER_SRGB);				//开启SRGB帧缓冲区进行Gamma矫正

	isRunning = true;
	return true;
}
void Pitaya::Engine::Renderer::OpenGLRenderer::ProcessRenderCmd()
{
	//Core::Log::Log(Core::Log::LogLevel::Debug, "渲染线程执行drawcallcmd" + std::to_string(renderCmd.DrawcallCmdCount()));
	drawcallCmdParser.ParseCmd();
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
		cond.wait(lock, [this] { return drawcallCmdParser.IsRemainCmd() || !isRunning; });

		ProcessRenderCmd();
		if (!drawcallCmdParser.IsRemainCmd() && !isRunning)
		{
			break;
		}
	}
}

void Pitaya::Engine::Renderer::OpenGLRenderer::Render()
{
	if (!isRunning)
	{
		return;
	}

	{	//准备drawcall命令
		std::lock_guard<std::mutex> lock(mutex);
		drawcallCmdParser.ClearDrawcallCmd();
		BeginFrame();
		DrawFrame();
		EndFrame();
	}

	//Core::Log::Log(Core::Log::LogLevel::Debug, "主线程提交DrawcallCmd");

	//唤醒渲染线程
	cond.notify_one();
}
void Pitaya::Engine::Renderer::OpenGLRenderer::BeginFrame()
{
	//TODO 更新本帧Camera的快照

	//TODO 渲染阴影贴图

	//TODO 切换到窗口帧帧率缓冲区
	//准备渲染 清空窗口帧缓冲区
	drawcallCmdParser.AddDrawcallCmd(Pitaya::Engine::Renderer::RenderQueue::A, [this]() {window->ClearFrameBuffer(); });
}
void Pitaya::Engine::Renderer::OpenGLRenderer::DrawFrame()
{
	//TODO 场景绘制
}
void Pitaya::Engine::Renderer::OpenGLRenderer::EndFrame()
{
	//TOOD 场景后处理

	//TODO UI绘制
	drawcallCmdParser.AddDrawcallCmd(Pitaya::Engine::Renderer::RenderQueue::C, [this]() {window->SwapBuffer(); });
}