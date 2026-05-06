#pragma once

#include<Core/RefCount/RefCount.h>
#include<GPU/Common/Identifier.h>
#include<GPU/Layout/BufferLayout.h>

namespace Pitaya::GPU
{
	class RHIDevice;
	struct VertexBuffer
	{
		struct Factory
		{
			friend class RHIDevice;
		private:
			static VertexBuffer Create(float* vertices, uint32_t size, BufferLayout layout);
			static void Destroy(VertexBuffer);
		};

		Pitaya::GPU::Identifier<VertexBuffer> Id = 0;
		Pitaya::GPU::BufferLayout layout;
	};
}
