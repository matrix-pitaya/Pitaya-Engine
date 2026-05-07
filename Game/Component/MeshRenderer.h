#pragma once

#include<Core/StateFlags/StateFlags.h>
#include<Core/Asset/Asset.h>
#include<Asset/Common/FuncTable.h>
#include<Asset/Common/Material.h>
#include<Asset/Common/Mesh.h>
#include<Render/Common/RenderLayer.h>

namespace Pitaya::Game
{
	struct MeshRenderer
	{
	public:
		inline void LoadMesh(Pitaya::Core::GUID meshGUID)
		{
			mesh = Pitaya::Asset::LoadAsset<Pitaya::Asset::Mesh>(meshGUID);
		}
		inline const Pitaya::Core::Asset<Pitaya::Asset::Mesh>& GetMesh() const noexcept
		{
			return mesh;
		}
		inline const std::vector<Pitaya::Core::Asset<Pitaya::Asset::Material>>& GetMaterials() const noexcept
		{
			static const std::vector<Pitaya::Core::Asset<Pitaya::Asset::Material>> Empty;
			return mesh.IsReady() ? mesh->Materials : Empty;
		}
		inline Pitaya::Render::RenderLayer GetLayerMask() const noexcept
		{
			return layerMask.GetEnum();
		}
		inline bool GetEnableShadowCast() const noexcept
		{
			return enableShadowCast;
		}
		inline void SetEnableShadowCast(bool flag) noexcept
		{
			enableShadowCast = flag;
		}
		inline bool GetReceiveShadow() const noexcept
		{
			return receiveShadow;
		}
		inline void SetReceiveShadow(bool flag) noexcept
		{
			receiveShadow = flag;
		}
		inline void SetLayerMask(Pitaya::Render::RenderLayer layer) noexcept
		{
			layerMask = layer;
		}

	private:
		Pitaya::Core::Asset<Pitaya::Asset::Mesh> mesh = nullptr;
		Pitaya::Core::StateFlags<Pitaya::Render::RenderLayer> layerMask = Pitaya::Render::RenderLayer::Default;
		bool enableShadowCast = true;
		bool receiveShadow = true;
	};
}
