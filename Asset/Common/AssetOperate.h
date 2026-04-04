#pragma once

#include<Import/Common/ImportResult.h>

namespace Pitaya::Asset
{
	struct Texture2DUnloadRequire
	{
		Pitaya::GPU::Identifier<Pitaya::GPU::Texture2D> ID = 0;
	};

	struct AssetOperate
	{
		std::variant<std::monostate,
			Pitaya::Import::Texture2DImportResult,
			Pitaya::Import::TextureCubemapImportResult,
			Pitaya::Import::ShaderImportResult,
			Pitaya::Import::StaticMeshImportResult,
			Pitaya::Import::SkinnedMeshImportResult,
			Pitaya::Import::RenderTargetImportResult,
			Pitaya::Asset::Texture2DUnloadRequire> Data;
	};
}
