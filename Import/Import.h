#pragma once

#include<Core/Identifier/GUID.h>
#include<Import/Common/ImportResult.h>
#include<Import/Common/TextureAPI.h>
#include<Import/Common/MeshAPI.h>

#include<filesystem>

namespace Pitaya::Import
{
	bool Import(Pitaya::Core::GUID guid, const std::filesystem::path& file, bool isFlipVerticallyOnLoad, bool isGenerateMipmap, Pitaya::Import::Texture2DImportResult& out, Pitaya::Import::TextureAPI api = Pitaya::Import::TextureAPI::StbImage);
	bool Import(Pitaya::Core::GUID guid, const std::filesystem::path& file, const std::vector<std::filesystem::path>& paths, bool isFlipVerticallyOnLoad, bool isGenerateMipmap, Pitaya::Import::TextureCubemapImportResult& out, Pitaya::Import::TextureAPI api = Pitaya::Import::TextureAPI::StbImage);
	bool Import(Pitaya::Core::GUID guid, const std::filesystem::path& folder, const std::filesystem::path& vertexPath, const std::filesystem::path& fragmentPath, Pitaya::Import::ShaderImportResult& out);
	bool Import(Pitaya::Core::GUID guid, const std::filesystem::path& folder, const std::filesystem::path& vertexPath, const std::filesystem::path& fragmentPath, const std::filesystem::path& geometryPath, Pitaya::Import::ShaderImportResult& out);
	bool Import(Pitaya::Core::GUID guid, const std::filesystem::path& file, Pitaya::Import::MeshPreloadResult& out, Pitaya::Import::MeshAPI api = Pitaya::Import::MeshAPI::Assimp);
	bool Import(Pitaya::Core::GUID guid, const std::filesystem::path& file, const Pitaya::Import::MeshPreloadResult& preload, Pitaya::Import::StaticMeshImportResult& out, Pitaya::Import::MeshAPI api = Pitaya::Import::MeshAPI::Assimp);
	bool Import(Pitaya::Core::GUID guid, const std::filesystem::path& file, const Pitaya::Import::MeshPreloadResult& preload, Pitaya::Import::SkinnedMeshImportResult& out, Pitaya::Import::MeshAPI api = Pitaya::Import::MeshAPI::Assimp);
}
