#pragma once

#include<cstdint>

namespace Pitaya::Editor
{
	enum class EngineState : uint8_t
	{
		Edit = 0,
		Play
	};
}