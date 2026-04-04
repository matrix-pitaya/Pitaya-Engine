#pragma once

#include<cstdint>

namespace Pitaya::Profiler
{
	struct TimeState
	{
		float delta = 0.0f;
		float Fixdelta = 1.0f / 60.0f;
		float UnscaledDeltaTime = 0.0f;
		float TimeScale = 1.0f;
		float FPS = 144.0f;
		double Seconds = 0.0f;
		int64_t Milliseconds = 0;

		inline void Reset() noexcept
		{
			delta = 0.0f;
			Fixdelta = 1.0f / 60.0f;
			UnscaledDeltaTime = 0.0f;
			TimeScale = 1.0f;
			FPS = 144.0f;
			Seconds = 0.0f;
			Milliseconds = 0;
		}
	};
}