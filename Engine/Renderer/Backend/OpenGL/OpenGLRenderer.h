#pragma once

#include<Enum/Enum.h>

#include<Engine/Interface/Renderer.h>
#include<Engine/API/Thread/Thread.h>

#include<Renderer/Runtime/Command/BeginPassCommand.h>
#include<Renderer/Runtime/Command/EndPassCommand.h>
#include<Renderer/Runtime/Command/DrawCommand.h>
#include<Renderer/Runtime/Command/SwapBufferCommand.h>
#include<Renderer/Runtime/Command/ImGuiDrawCommand.h>

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
		bool Initialize() override;
		void Release() override;

	private:
		void SwapBuffer() const override;
		void BeginRenderFrame() override;
		void EndRenderFrame() override;
		void BeginPass(const Pitaya::Engine::Renderer::CameraSnapshot&) override;
		void EndPass() override;
		void Submit() override;

		Pitaya::Engine::Renderer::VertexArray* CreateVertexArray() override;
		Pitaya::Engine::Renderer::VertexBuffer* CreateVertexBuffer(float* vertices, uint32_t size) override;
		Pitaya::Engine::Renderer::ElementBuffer* CreateElementBuffer(uint32_t* indices, uint32_t count) override;

	private:
		void RenderThread();
		bool InitOpenGLContext();

	private:
		inline void ParseCommand(size_t alignTo = 16)
		{
			size_t offset = 0;
			while (offset < back.size())
			{
				size_t padding = (alignTo - (offset % alignTo)) % alignTo;
				offset += padding;

				if (offset + sizeof(CommandHeader) > back.size())
				{
					break;
				}

				const CommandHeader* header = reinterpret_cast<const CommandHeader*>(&back[offset]);
				offset += sizeof(CommandHeader);

				switch (header->type)
				{
					case RenderCommandType::BeginPass:
						ExecuteCommand(FetchCommandFromBackBuffer<BeginPassCommand>(offset));
						break;

					case RenderCommandType::EndPass:
						ExecuteCommand(FetchCommandFromBackBuffer<EndPassCommand>(offset));
						break;

					case RenderCommandType::Draw:
						ExecuteCommand(FetchCommandFromBackBuffer<DrawCommand>(offset));
						break;

					case RenderCommandType::ImGuiDraw:
						ExecuteCommand(FetchCommandFromBackBuffer<ImGuiDrawCommand>(offset));
						break;

					case RenderCommandType::SwapBuffer:
						ExecuteCommand(FetchCommandFromBackBuffer<SwapBufferCommand>(offset));
						break;

					case RenderCommandType::Unknown:
						offset += header->size;
						break;

					default:
						offset += header->size;
						return;
				}
			}
			back.clear();
		}
		inline void ExecuteCommand(const BeginPassCommand* command) const
		{
			if (!command)
			{
				return;
			}

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
		inline void ExecuteCommand(const EndPassCommand* command) const
		{

		}
		inline void ExecuteCommand(const DrawCommand* command) const
		{

		}
		inline void ExecuteCommand(const SwapBufferCommand* command) const
		{
			SwapBuffer();
		}
		inline void ExecuteCommand(const ImGuiDrawCommand* command) const
		{

		}

	private:
		struct CommandHeader 
		{
			CommandHeader(RenderCommandType type = RenderCommandType::Unknown, uint32_t size = 0)
				:type(type), size(size) {}

			RenderCommandType type = RenderCommandType::Unknown;
			uint32_t size = 0;
		};
		template <typename T>
		void PushCommandToFrontBuffer(RenderCommandType type, const T& obj, size_t alignTo = 16)
		{
			static_assert(std::is_trivially_copyable<T>::value,
				"Object must be trivially copyable (POD-like) for raw memory copy!");

			size_t totalSize = sizeof(CommandHeader) + sizeof(T);

			size_t currentSize = front.size();
			size_t padding = (alignTo - (currentSize % alignTo)) % alignTo;
			if (padding > 0)
			{
				front.insert(front.end(), padding, 0);
			}

			CommandHeader header = CommandHeader(type, (uint32_t)sizeof(T));
			const uint8_t* hPtr = reinterpret_cast<const uint8_t*>(&header);
			front.insert(front.end(), hPtr, hPtr + sizeof(CommandHeader));

			const uint8_t* dPtr = reinterpret_cast<const uint8_t*>(&obj);
			front.insert(front.end(), dPtr, dPtr + sizeof(T));
		}
		template <typename T>
		const T* FetchCommandFromBackBuffer(size_t& offset)
		{
			if (offset + sizeof(T) > back.size())
			{
				return nullptr;
			}

			const T* ptr = reinterpret_cast<const T*>(&back[offset]);
			offset += sizeof(T);
			return ptr;
		}

	private:
		std::condition_variable cond;
		std::atomic<bool> isRunning = false;
		std::mutex mutex;

		Pitaya::Engine::Thread::ThreadToken renderThreadToken;

		std::vector<uint8_t> front;			//主线程写入渲染命令
		std::vector<uint8_t> back;			//渲染线程执行渲染命令
		std::vector<DrawCommand> pass;		//用于对DrawCommand进行排序

		GLFWwindow* glfwWindow = nullptr;
	};
}