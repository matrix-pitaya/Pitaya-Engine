#pragma once

#include<Core/StateFlags/StateFlags.h>
#include<Render/Common/RenderLayer.h>
#include<Asset/Common/Material.h>
#include<Asset/Common/Mesh.h>

#include<glm.hpp>
#include<vector>

namespace Pitaya::Render
{
	struct RenderItem
	{
		Pitaya::Asset::Mesh* Mesh = nullptr;
		Pitaya::Asset::Material* Material = nullptr;
		Pitaya::Core::StateFlags<Pitaya::Render::RenderLayer> LayerMask;
		glm::mat4 Model;
		uint32_t SubMeshIndex = 0;
		bool EnableShadowCast = true;
		bool ReceiveShadow = true;
	};
}
