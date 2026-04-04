#pragma once

#include<Core/RefCount/RefCount.h>
#include<GPU/Common/Identifier.h>
#include<GPU/Layout/BufferLayout.h>

namespace Pitaya::GPU
{
	class RHIDevice;
	class VertexBuffer : public Pitaya::Core::RefCount
	{
		friend class Pitaya::GPU::RHIDevice;
	public:
		VertexBuffer(float* vertices, uint32_t size) {}
		virtual ~VertexBuffer() = default;

	public:
		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;
		virtual Identifier<VertexBuffer> GetGPUIdentifier() const = 0;

	public:
		inline void SetLayout(const BufferLayout& layout)
		{
			this->layout = layout;
		}
		inline const BufferLayout& GetLayout() const
		{
			return layout;
		}

	private:
		static VertexBuffer* Create(float* vertices, uint32_t size);

	protected:
		Pitaya::GPU::BufferLayout layout;
	};
}
