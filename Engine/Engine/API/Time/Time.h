#pragma once

#include<functional>

namespace Pitaya::Engine::Time
{
	float delta() noexcept;
	float Fixdelta() noexcept;
	float UnscaledDelta() noexcept;
	float TimeScale() noexcept;
	float Seconds() noexcept;
	int64_t Milliseconds() noexcept;
}