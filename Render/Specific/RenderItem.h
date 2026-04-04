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

		RenderItem(Pitaya::Asset::Mesh* mesh, Pitaya::Asset::Material* material, Pitaya::Render::RenderLayer layerMask, const glm::mat4& model, uint32_t subMeshIndex)
			:Mesh(mesh), Material(material), LayerMask(layerMask), Model(model), SubMeshIndex(subMeshIndex){}
	};
}
