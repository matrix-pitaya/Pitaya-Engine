#pragma once

#include<Core/Container/SlotMap.h>
#include<GPU/Common/GPUObjectType.h>
#include<GPU/Common/TextureUsage.h>

#include<glm.hpp>
#include<algorithm>

namespace Pitaya::Render
{
    inline constexpr const uint64_t MaxSortKey = 0xFFFFFFFFFFFFFFFF;
    inline constexpr const uint64_t MinSortKey = 0;

    inline constexpr const uint64_t RENDER_QUEUE_SHIFT = 60;
    inline constexpr const uint64_t DRAW_ORDER_SHIFT = 52;
    inline constexpr const uint64_t SHADER_ID_SHIFT = 42;
    inline constexpr const uint64_t MATERIAL_ID_SHIFT = 30;
    inline constexpr const uint64_t MESH_ID_SHIFT = 16;
    inline constexpr const uint64_t DEPTH_SHIFT = 0;

	//[Queue][Order][Shader][Material][Mesh][Depth]
    inline uint64_t GenerateSortKey(RenderQueue renderQueue, int32_t drawOrder, uint32_t shaderID, uint32_t materialID, uint32_t meshID, float depth)
    {
        uint64_t key = 0;
        // Queue (Mask 0xF)
        key |= (static_cast<uint64_t>(renderQueue) & 0xF) << RENDER_QUEUE_SHIFT;
        
        // DrawOrder (Mask 0xFF)
        uint32_t orderVal = static_cast<uint32_t>(drawOrder + 128);
        key |= (static_cast<uint64_t>(orderVal) & 0xFF) << DRAW_ORDER_SHIFT;
        
        // Shader (Mask 0x3FF -> 1023)
        key |= (static_cast<uint64_t>(shaderID) & 0x3FF) << SHADER_ID_SHIFT;
        
        // Material (Mask 0xFFF -> 4095)
        key |= (static_cast<uint64_t>(materialID) & 0xFFF) << MATERIAL_ID_SHIFT;
        
        // Mesh (Mask 0x3FFF -> 16383)
        key |= (static_cast<uint64_t>(meshID) & 0x3FFF) << MESH_ID_SHIFT;
        
        // Depth (Mask 0xFFFF)
        float normalizedDepth = std::clamp((depth + 1.0f) * 0.5f, 0.0f, 1.0f);
        uint32_t depthInt = (renderQueue <= RenderQueue::Geometry) ?
            static_cast<uint32_t>(normalizedDepth * 0xFFFF) :
            depthInt = static_cast<uint32_t>((1.0f - normalizedDepth) * 0xFFFF);
        key |= (static_cast<uint64_t>(depthInt) & 0xFFFF) << DEPTH_SHIFT;
        return key;
    }

	struct DrawCommand
	{
        Pitaya::Core::SlotMap<Pitaya::GPU::VertexArray>::Handle VertexArrayHandle;
        Pitaya::Core::SlotMap<Pitaya::GPU::Shader>::Handle ShaderHandle;
        Pitaya::Core::SlotMap<Pitaya::GPU::Texture2D>::Handle TextureHandles[static_cast<size_t>(Pitaya::GPU::TextureUsage::Unknown)] = {};

		uint64_t SortKey = 0;

		uint32_t MaterialId = 0;
		uint32_t IndexCount = 0;
		uint32_t BaseIndex = 0;
		uint32_t BaseVertex = 0;
		
		glm::mat4 ModelMatrix;
		std::vector<glm::mat4> BoneInverseMatrices;

        bool DepthTest = true;
        bool Blend = false;
        bool CullFace = true;
	};
}
