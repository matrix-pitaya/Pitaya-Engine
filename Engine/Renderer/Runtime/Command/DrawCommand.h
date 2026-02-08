#pragma once

#include<glm.hpp>

#include<algorithm>

namespace Pitaya::Engine::Renderer
{
    inline constexpr const uint64_t MaxSortKey = 0xFFFFFFFFFFFFFFFF;
    inline constexpr const uint64_t MinSortKey = 0;

    inline constexpr const uint64_t RENDER_QUEUE_SHIFT = 60;  // 4位  (60-63)
    inline constexpr const uint64_t SHADER_ID_SHIFT = 48;	  // 12位 (48-59)
    inline constexpr const uint64_t MATERIAL_ID_SHIFT = 32;   // 16位 (32-47)
    inline constexpr const uint64_t DEPTH_SHIFT = 8;		  // 24位 (8-31)
    inline constexpr const uint64_t DRAW_ORDER_SHIFT = 0;	  // 8位  (0-7)

	//[RenderQueue][Shader][Material][Depth]
	inline uint64_t GenerateSortKey(RenderQueue renderQueue, uint32_t shaderID, uint32_t materialID, float depth, uint8_t drawOrder = 0)
	{
		uint64_t key = 0;

		//1.Queue(最高优先级)
		key |= (static_cast<uint64_t>(renderQueue) & 0xF) << RENDER_QUEUE_SHIFT;

		//2.Shader(减少渲染管线切换)
		key |= (static_cast<uint64_t>(shaderID) & 0xFFF) << SHADER_ID_SHIFT;

		//3.Material/Texture
		key |= (static_cast<uint64_t>(materialID) & 0xFFFF) << MATERIAL_ID_SHIFT;

		//4.Depth处理
		//NDC 深度通常在[0, 1]或[-1, 1] 映射到 24位整数[0, 16777215]
		float normalizedDepth = (depth + 1.0f) * 0.5f;
		normalizedDepth = std::clamp(normalizedDepth, 0.0f, 1.0f);

		uint32_t depthInt = 0;
		if (renderQueue <= RenderQueue::Opaque)
		{
			//不透明:近(0)->远(1) Key越小越靠前 利用Early-Z
			depthInt = static_cast<uint32_t>(normalizedDepth * 0xFFFFFF);
		}
		else
		{
			//半透明:远(1)->近(0) 确保Alpha Blending正确 必须从远往近画 所以远处的物体的SortKey较小
			depthInt = static_cast<uint32_t>((1.0f - normalizedDepth) * 0xFFFFFF);
		}
		key |= (static_cast<uint64_t>(depthInt) & 0xFFFFFF) << DEPTH_SHIFT;

		//5.Order
		key |= (static_cast<uint64_t>(drawOrder) & 0xFF) << DRAW_ORDER_SHIFT;

		return key;
	}

	struct DrawCommand
	{
        uint32_t VertexArray = 0;
        uint32_t Shader = 0;
        uint32_t Texture = 0;
        uint32_t IndexCount = 0;
        uint64_t SortKey = 0;

        glm::mat4 ModelMatrix;

        bool DepthTest = true;
        bool Blend = false;
        bool Wireframe = false;
	};
}