#pragma once

#include<Core/Container/SlotMap.h>
#include<GPU/Common/GPUObjectType.h>

#include<glm.hpp>

namespace Pitaya::Asset { struct Material; }

namespace Pitaya::Render
{
	struct DrawCommand
	{
	    Pitaya::Core::SlotMap<Pitaya::GPU::VertexArray>::Handle VertexArrayHandle;
	    Pitaya::Core::SlotMap<Pitaya::GPU::Shader>::Handle ShaderHandle;

		uint64_t SortKey = 0;

		uint32_t MaterialId = 0;
		uint32_t IndexCount = 0;
		uint32_t BaseIndex = 0;
		uint32_t BaseVertex = 0;

		glm::mat4 ModelMatrix;
		const std::vector<glm::mat4>* BoneInverseMatrices = nullptr;
		const Pitaya::Asset::Material* MaterialPtr = nullptr;

	    bool DepthTest = true;
	    bool Blend = false;
	    bool CullFace = true;
		bool ReceiveShadow = true;
	};
}
