#pragma once

#include<Core/RefCount/RefCount.h>
#include<GPU/Common/Identifier.h>

#include<cstdint>

namespace Pitaya::GPU
{
	class RHIDevice;
	class IndexBuffer : public Pitaya::Core::RefCount
	{
		friend class Pitaya::GPU::RHIDevice;
	public:
		IndexBuffer(uint32_t* indices, uint32_t count)
			:count(count) { }
		virtual ~IndexBuffer() = default;

	public:
		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;
		virtual Pitaya::GPU::Identifier<IndexBuffer> GetGPUIdentifier() const = 0;

	public:
		inline uint32_t GetCount() const noexcept
		{
			return count;
		}

	private:
		static Pitaya::GPU::IndexBuffer* Create(uint32_t* indices, uint32_t count);

	protected:
		uint32_t count = 0;
	};
}
