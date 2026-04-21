#pragma once

#include<Core/Asset/Asset.h>
#include<Asset/Common/Material.h>

namespace Pitaya::Game
{
	struct MaterialOverride
	{
		inline std::vector<Pitaya::Core::Asset<Pitaya::Asset::Material>>& GetOverrideMaterials() noexcept
		{
			return overrideMaterials;
		}

	private:
		std::vector<Pitaya::Core::Asset<Pitaya::Asset::Material>> overrideMaterials;
	};
}