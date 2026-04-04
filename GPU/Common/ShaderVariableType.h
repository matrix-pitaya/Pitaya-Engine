#pragma once

#include<cstdint>

namespace Pitaya::GPU
{
	enum class ShaderVariableType : uint8_t
	{
		Float = 0,
		Float2,
		Float3,
		Float4,
		Mat3,
		Mat4,
		Int,
		Int2,
		Int3,
		Int4,
		Bool,

		Unknown
	};
}
