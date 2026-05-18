#pragma once

#include<cstdint>
#include<vector>

namespace Pitaya::Asset
{
	enum class ParamType : uint8_t
	{
		Vector,
		Float,
		Texture,

		Invalid
	};

	struct ParamSlot
	{
		const char* Name = nullptr;
		ParamType Type = ParamType::Invalid;
		uint32_t Index = 0;
		uint32_t Offset = 0;
		uint32_t Size = 0;
	};

	struct MaterialParamLayout
	{
		std::vector<ParamSlot> Slots;
		uint32_t VectorCount = 0;
		uint32_t FloatCount = 0;
		uint32_t TextureCount = 0;
		uint32_t TotalBytes = 0;
	};
}
