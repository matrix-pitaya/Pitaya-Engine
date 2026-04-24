#pragma once

#include<Render/Frontend/Renderer.h>

#include<GL/glew.h>
#include<GLFW/glfw3.h>

#include<gtc/type_ptr.hpp>

namespace Pitaya::Render
{
	class OpenGLRenderer : public Pitaya::Render::Renderer
	{
		friend class Pitaya::Render::Renderer;
	private:
		OpenGLRenderer() = default;
		~OpenGLRenderer() override = default;

	public:
		OpenGLRenderer(const OpenGLRenderer&) = delete;
		OpenGLRenderer& operator=(const OpenGLRenderer&) = delete;
		OpenGLRenderer(OpenGLRenderer&&) = delete;
		OpenGLRenderer& operator=(OpenGLRenderer&&) = delete;

	private:
		bool InitializeRenderContext(void* nativeWindow) override;
		void ReleaseRenderContext() override;

	private:
		void SwapBuffer() const override;
		void NewRenderFrame() override;

	private:
		void ExecuteCommand(const Pitaya::Render::BeginPassCommand* command) const override;
		void ExecuteCommand(const Pitaya::Render::InstancedDrawCommand* command) const override;
		void ExecuteCommand(const Pitaya::Render::PostProcessCommand* command) const override;
		void ExecuteCommand(const Pitaya::Render::BlitToScreenCommand* command) const override;

	private:
		GLFWwindow* glfwWindow = nullptr;
	};
}
