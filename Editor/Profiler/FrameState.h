#pragma once

#include<Editor/Profiler/TimeState.h>
#include<Editor/Profiler/RenderState.h>

namespace Pitaya::Editor
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