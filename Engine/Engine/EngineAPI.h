#pragma once

#include<string>
#include<functional>

namespace Pitaya::Engine
{
	int Execute();
}

namespace Pitaya::Engine::Config
{
	size_t GetMaxFixupdataExecuteTimes();
}

namespace Pitaya::Engine::Time
{
	extern const float& delta;
	extern const float& fixdelta;
	extern const float& unscaledDeltaTime;
	extern float& scale;

	float Seconds();
	int64_t Milliseconds();
}
