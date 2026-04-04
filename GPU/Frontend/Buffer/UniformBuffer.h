#pragma once

#include<GPU/Common/Identifier.h>

namespace Pitaya::GPU
{
	class RHIDevice;
	class UniformBuffer
	{
		friend class Pitaya::GPU::RHIDevice;
	public:
		UniformBuffer(uint32_t size, uint32_t bindingPoint)
			:size(size), bindingPoint(bindingPoint) {}
		virtual ~UniformBuffer() = default;

	public:
		virtual void SetData(const void* data, uint32_t size, uint32_t offset = 0) const = 0;
		virtual Pitaya::GPU::Identifier<UniformBuffer> GetGPUIdentifier() const = 0;

	public:
		inline uint32_t GetSize() const noexcept
		{
			return size;
		}
		inline uint32_t GetBindingPoint() const noexcept
		{
			return bindingPoint;
		}

	private:
		static Pitaya::GPU::UniformBuffer* Create(uint32_t size, uint32_t bindingPoint);

	protected:
		uint32_t size = 0;
		uint32_t bindingPoint = 0;
	};
}
