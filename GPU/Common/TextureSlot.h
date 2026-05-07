#pragma once

#include<cstdint>

namespace Pitaya::GPU
{
	enum class TextureSlot : uint8_t
	{
		Albedo = 0,       //基础色
		Specular,         //高光反射纹理
		Normal,           //法线
		Metallic,         //金属度
		Roughness,        //粗糙度
		AmbientOcclusion, //AO 环境光遮蔽
		Emission,         //自发光
		Height,           //高度/位移
		Mask,             //遮罩

		CSM,
		SPOT,
		POINT
	};

	inline constexpr const size_t MaterialTextureSlotCount = static_cast<size_t>(TextureSlot::CSM);
	inline constexpr const size_t ShadowTextureSlotCount = static_cast<size_t>(TextureSlot::POINT) - static_cast<size_t>(TextureSlot::CSM) + 1;
}
