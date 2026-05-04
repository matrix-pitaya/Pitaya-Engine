#pragma once

#include<GPU/Common/Identifier.h>

namespace Pitaya::GPU
{
	class RHIDevice;
	struct UniformBuffer
	{
		struct Factory
		{
			friend class RHIDevice;
		private:
			static UniformBuffer Create(uint32_t size, uint32_t bindingPoint);
			static void Destroy(UniformBuffer);
		};

		Pitaya::GPU::Identifier<UniformBuffer> Id = 0;;
		uint32_t size = 0;
		uint32_t bindingPoint = 0;
	};
}
