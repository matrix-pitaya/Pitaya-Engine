#pragma once

#include"Enum/Enum.h"

#include"../../Engine/Engine/Interface/Renderer.h"
#include"../../Engine/Engine/Internal/Thread/Thread.h"

#include<atomic>
#include<condition_variable>
#include<mutex>
#include<vector>
#include<functional>
#include<algorithm>

#include<GL/glew.h>
#include<GLFW/glfw3.h>

namespace Pitaya::Engine
{
	class Engine;
}

namespace Pitaya::Engine::Renderer
{
	class OpenGLRenderer : public Pitaya::Engine::Interface::Renderer
	{
		friend class Pitaya::Engine::Engine;
	private:
		OpenGLRenderer() = default;
		~OpenGLRenderer() override = default;

	public:
		OpenGLRenderer(const OpenGLRenderer&) = delete;
		OpenGLRenderer& operator=(const OpenGLRenderer&) = delete;
		OpenGLRenderer(OpenGLRenderer&&) = delete;
		OpenGLRenderer& operator=(OpenGLRenderer&&) = delete;

	private:
		class DrawcallCmdParser
		{
		private:
			struct DrawcallCmd
			{
				DrawcallCmd(Pitaya::Engine::Renderer::RenderQueue renderQueue, std::function<void()> drawcall)
					:renderQueue(renderQueue), drawcall(std::move(drawcall)) { }

				Pitaya::Engine::Renderer::RenderQueue renderQueue;
				std::function<void()> drawcall;
			};

		public:
			DrawcallCmdParser() = default;
			~DrawcallCmdParser() = default;
			DrawcallCmdParser(const DrawcallCmdParser&) = delete;
			DrawcallCmdParser& operator=(const DrawcallCmdParser&) = delete;
			DrawcallCmdParser(DrawcallCmdParser&&) = delete;
			DrawcallCmdParser& operator=(DrawcallCmdParser&&) = delete;

			inline void ParseCmd()
			{
				//根据渲染队列进行排序
				std::sort(drawcallCmds.begin(), drawcallCmds.end(),
					[](const DrawcallCmd& a, const DrawcallCmd& b) { return a.renderQueue < b.renderQueue; });
				for (auto& drawcallCmd : drawcallCmds)
				{
					if (drawcallCmd.drawcall)
					{
						drawcallCmd.drawcall();
					}
				}
				drawcallCmds.clear();
			}
			inline void ClearDrawcallCmd()
			{
				drawcallCmds.clear();
			}
			inline void AddDrawcallCmd(Pitaya::Engine::Renderer::RenderQueue renderQueue, std::function<void()> drawcall)
			{
				drawcallCmds.emplace_back(renderQueue, std::move(drawcall));
			}
			inline bool IsRemainCmd() const noexcept
			{
				return !drawcallCmds.empty();
			}
			inline size_t DrawcallCmdCount() const noexcept
			{
				return drawcallCmds.size();
			}

		private:
			std::vector<DrawcallCmd> drawcallCmds;
		};

	public:
		bool Initialize() override;
		void Release() override;
		void Render() override;

	private:
		void BeginFrame();
		void DrawFrame();
		void EndFrame();

#pragma region RenderThreadFunction
	private:
		void RenderThread();
		bool InitOpenGLContext();
		void ProcessRenderCmd();
#pragma endregion

	private:
		std::condition_variable cond;
		std::atomic<bool> isRunning = false;
		std::mutex mutex;

		Pitaya::Engine::Thread::ThreadToken renderThreadToken;

		DrawcallCmdParser drawcallCmdParser;

		GLFWwindow* openGLWindow = nullptr;
	};
}