#pragma once

#include<cstdint>

namespace Pitaya::Editor
{
	struct DrawcallState
	{
		uint32_t BeforeBatchDrawcallTimes = 0;
		uint32_t AfterBatchDrawcallTimes = 0;
	};
	struct RenderPassState
	{
		
	};
	struct RenderState
	{
		uint32_t RenderObjectCount = 0;
		uint32_t RenderPassCount = 0;



		inline void Reset() noexcept
		{
			RenderObjectCount = 0;
			RenderPassCount = 0;
		}
	};
}