#pragma once

#include<Import/TextureImporter/Frontend/TextureImporter.h>

namespace Pitaya::Import
{
	class StbImageTextureImporter : public TextureImporter
	{
	public:
		StbImageTextureImporter() = default;
		~StbImageTextureImporter() override = default;

	public:
		bool Import(Pitaya::Core::GUID guid, const std::filesystem::path& file, bool isFlipVerticallyOnLoad, bool isGenerateMipmap, Pitaya::Import::Texture2DImportResult& out) override;
		bool Import(Pitaya::Core::GUID guid, const std::filesystem::path& file,  const std::vector<std::filesystem::path>& paths, bool isFlipVerticallyOnLoad, bool isGenerateMipmap, Pitaya::Import::TextureCubemapImportResult& out) override;
		bool Import(Pitaya::Core::GUID guid, const std::filesystem::path& file, SkyBoxImportResult& out) override;
		bool Import(Pitaya::Core::GUID guid, const void* data, size_t size, SkyBoxImportResult& out) override;
	};
}
