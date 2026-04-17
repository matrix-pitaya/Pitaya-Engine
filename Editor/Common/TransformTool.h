#pragma once

#include<cstdint>

namespace Pitaya::Editor
{
	enum class TransformTool : uint8_t
	{
		Select = 0,
		Translate,
		Rotate,
		Scale
	};
}