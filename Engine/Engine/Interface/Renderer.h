#pragma once

#include<Engine/Interface/Window.h>

#include<Renderer/Runtime/Component/Camera/CameraSnapshot.h>

#include<Renderer/Runtime/Vertex/VertexArray.h>
#include<Renderer/Runtime/Vertex/VertexBuffer.h>
#include<Renderer/Runtime/Vertex/ElementBuffer.h>

namespace Pitaya::Engine
{
	class Engine;
}

namespace Pitaya::Engine::Interface
{
	class Renderer
	{
		friend class Pitaya::Engine::Engine;
	protected:
		Renderer() = default;
		virtual ~Renderer() = default;

	public:
		Renderer(const Renderer&) = delete;
		Renderer& operator=(const Renderer&) = delete;
		Renderer(Renderer&&) = delete;
		Renderer& operator=(Renderer&&) = delete;

	protected:
		virtual bool Initialize() = 0;
		virtual void Release() = 0;

	protected:
		virtual	void SwapBuffer() const = 0;
		virtual void BeginRenderFrame() = 0;
		virtual void EndRenderFrame() = 0;
		virtual void BeginPass(const Pitaya::Engine::Renderer::CameraSnapshot&) = 0;
		virtual void EndPass() = 0;
		virtual void Submit() = 0;

		virtual Pitaya::Engine::Renderer::VertexArray* CreateVertexArray() = 0;
		virtual Pitaya::Engine::Renderer::VertexBuffer* CreateVertexBuffer(float* vertices, uint32_t size) = 0;
		virtual Pitaya::Engine::Renderer::ElementBuffer* CreateElementBuffer(uint32_t* indices, uint32_t count) = 0;

	protected:
		inline void Bind(Pitaya::Engine::Interface::Window* window) noexcept
		{
			this->window = window;
		}

	protected:
		Pitaya::Engine::Interface::Window* window = nullptr;
	};
}