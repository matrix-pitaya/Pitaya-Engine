#pragma once

#include<Profiler/Common/TimeState.h>
#include<Profiler/Common/RenderState.h>

namespace Pitaya::Profiler
{
	struct FrameState
	{
		TimeState Time;
		RenderState Render;

		inline void Reset() noexcept
		{
			Time.Reset();
			Render.Reset();
		}
	};
}