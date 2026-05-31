#pragma once

#include<Core/Asset/AssetRef.h>
#include<Asset/Common/Material.h>

namespace Pitaya::Game
{
	struct MaterialOverride
	{
		inline std::vector<Pitaya::Core::AssetRef<Pitaya::Asset::Material>>& GetOverrideMaterials() noexcept
		{
			return overrideMaterials;
		}

	private:
		std::vector<Pitaya::Core::AssetRef<Pitaya::Asset::Material>> overrideMaterials;
	};
}