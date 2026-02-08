#pragma once

#include<cstdint>

namespace Pitaya::Engine::Renderer
{
	class ElementBuffer
	{
	public:
		ElementBuffer() = default;
		virtual ~ElementBuffer() = default;

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;
		virtual uint32_t GetCount() const = 0;
	};
}