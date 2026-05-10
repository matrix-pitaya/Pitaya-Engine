#pragma once

#include<Core/Layout/def.h>

#include<glm.hpp>
#include<cstdint>

namespace Pitaya::Render
{
	inline constexpr const uint32_t CSM_CASCADE_COUNT = 4;
	inline constexpr const uint32_t POINT_SHADOW_FACE_COUNT = 6;

	// CPU侧 每帧阴影元数据
	struct ShadowCasterSlice
	{
		uint32_t LightType;      // 0=Dir, 1=Point, 2=Spot
		uint32_t LightIndex;     // 在 Lights[] 中的索引
		uint32_t PassIndex;      // 对应的 RenderPass 索引（CSM 用，Spot/Point 填 UINT32_MAX）
		uint32_t MatrixOffset;   // 在 ShadowMatrices[] 中的起始下标
		uint32_t MatrixCount;    // 4 / 6 / 1
		uint32_t LayerOffset;    // 在对应 Texture2DArray 中的起始 layer
		float NearPlane;
		float FarPlane;
	};

	// GPU侧 上传到 SSBO 的数据
	// [SSBO layout]
	// ShadowSSBOHeader                                    (4 × uint + 4 × uint padding → 32 bytes)
	// CascadeSplitInfo[dirLightCount × passCount]         (16 bytes each)
	// ShadowSliceGPU[totalSliceCount]                     (16 bytes each)
	// mat4 ShadowMatrices[totalMatrixCount]               (64 bytes each)

	struct STD140_LAYOUT ShadowSliceGPU
	{
		uint32_t MatrixOffset;		// 在 ShadowMatrices 数组中的起始下标
		uint32_t LayerOffset;		// 在对应 Texture2DArray 中的起始 layer
		uint32_t LightType;			// 0=Dir, 1=Point, 2=Spot
		uint32_t _pad0;
	};

	struct STD140_LAYOUT ShadowSSBOHeader
	{
		uint32_t DirectionalLightCount;
		uint32_t SpotLightCount;
		uint32_t PointLightCount;
		uint32_t TotalMatrixCount;
		uint32_t CascadeSplitCount;
		uint32_t ShadowSliceCount;
		uint32_t _pad0;
		uint32_t _pad1;
	};

	struct CascadeSplitInfo
	{
		glm::vec4 Distances;  // x,y,z,w = cascade 0~3 的远裁剪面距离（view space）
	};
}