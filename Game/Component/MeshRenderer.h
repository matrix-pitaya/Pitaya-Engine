#pragma once

#include<Core/StateFlags/StateFlags.h>
#include<Asset/Common/FuncTable.h>
#include<Render/Common/RenderLayer.h>

namespace Pitaya::Game
{
	struct MeshRenderer
	{
	public:
		MeshRenderer() = default;
		~MeshRenderer() = default;
	
	public:
		inline Pitaya::Core::Asset<Pitaya::Asset::Mesh>& GetMesh() noexcept
		{
			return mesh;
		}
		inline std::vector<Pitaya::Core::Asset<Pitaya::Asset::Material>>& GetMaterials() noexcept
		{
			return materials;
		}
		inline Pitaya::Render::RenderLayer GetLayerMask() const noexcept
		{
			return layerMask.GetEnum();
		}

	public:
		Pitaya::Core::Asset<Pitaya::Asset::Mesh> mesh = nullptr;
		std::vector<Pitaya::Core::Asset<Pitaya::Asset::Material>> materials;
		Pitaya::Core::StateFlags<Pitaya::Render::RenderLayer> layerMask = Pitaya::Render::RenderLayer::Default;
	};
}
