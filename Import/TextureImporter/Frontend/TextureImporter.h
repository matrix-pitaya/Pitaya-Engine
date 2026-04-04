#pragma once

#include<Import/Common/ImportResult.h>

#include<filesystem>

namespace Pitaya::Import
{
	class TextureImporter
	{
	public:
		TextureImporter() = default;
		virtual ~TextureImporter() = default;

	public:
		virtual bool Import(Pitaya::Core::GUID guid, const std::filesystem::path& file, bool isFlipVerticallyOnLoad, bool isGenerateMipmap, Texture2DImportResult& out) = 0;
		virtual bool Import(Pitaya::Core::GUID guid, const std::filesystem::path& file, const std::vector<std::filesystem::path>& paths, bool isFlipVerticallyOnLoad, bool isGenerateMipmap, TextureCubemapImportResult& out) = 0;
	};
}
