#pragma once

#include<Core/RefCount/RefCount.h>
#include<GPU/Common/Identifier.h>

#include<cstdint>

namespace Pitaya::GPU
{
	class RHIDevice;
	struct IndexBuffer
	{
		struct Factory
		{
			friend class RHIDevice;
		private:
			static IndexBuffer Create(const uint32_t* indices, uint32_t count);
			static void Destroy(IndexBuffer);
		};

		Pitaya::GPU::Identifier<IndexBuffer> Id = 0;
		uint32_t count = 0;
	};
}
