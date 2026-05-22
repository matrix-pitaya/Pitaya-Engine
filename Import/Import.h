#pragma once

#include<Core/Identifier/GUID.h>
#include<Import/Common/ImportResult.h>
#include<Import/Common/TextureAPI.h>
#include<Import/Common/MeshAPI.h>

#include<Engine/API/def.h>

#include<filesystem>

namespace Pitaya::Import
{
	ENGINE_API bool ENGINE_CALL Import(Pitaya::Core::GUID guid, const std::filesystem::path& file, bool isFlipVerticallyOnLoad, bool isGenerateMipmap, Pitaya::Import::Texture2DImportResult& out, Pitaya::Import::TextureAPI api = Pitaya::Import::TextureAPI::StbImage);
	ENGINE_API bool ENGINE_CALL Import(Pitaya::Core::GUID guid, const std::filesystem::path& file, const std::vector<std::filesystem::path>& paths, bool isFlipVerticallyOnLoad, bool isGenerateMipmap, Pitaya::Import::TextureCubemapImportResult& out, Pitaya::Import::TextureAPI api = Pitaya::Import::TextureAPI::StbImage);
	ENGINE_API bool ENGINE_CALL Import(Pitaya::Core::GUID guid, const std::filesystem::path& folder, const std::filesystem::path& vertexPath, const std::filesystem::path& fragmentPath, Pitaya::Import::ShaderImportResult& out);
	ENGINE_API bool ENGINE_CALL Import(Pitaya::Core::GUID guid, const std::filesystem::path& folder, const std::filesystem::path& vertexPath, const std::filesystem::path& fragmentPath, const std::filesystem::path& geometryPath, Pitaya::Import::ShaderImportResult& out);
	ENGINE_API bool ENGINE_CALL Import(Pitaya::Core::GUID guid, const std::filesystem::path& file, Pitaya::Import::MeshPreloadResult& out, Pitaya::Import::MeshAPI api = Pitaya::Import::MeshAPI::Assimp);
	ENGINE_API bool ENGINE_CALL Import(Pitaya::Core::GUID guid, const std::filesystem::path& file, const Pitaya::Import::MeshPreloadResult& preload, Pitaya::Import::StaticMeshImportResult& out, Pitaya::Import::MeshAPI api = Pitaya::Import::MeshAPI::Assimp);
	ENGINE_API bool ENGINE_CALL Import(Pitaya::Core::GUID guid, const std::filesystem::path& file, const Pitaya::Import::MeshPreloadResult& preload, Pitaya::Import::SkinnedMeshImportResult& out, Pitaya::Import::MeshAPI api = Pitaya::Import::MeshAPI::Assimp);
	ENGINE_API bool ENGINE_CALL Import(Pitaya::Core::GUID guid, const std::filesystem::path& file, Pitaya::Import::SkyBoxImportResult& out);
	ENGINE_API bool ENGINE_CALL Import(Pitaya::Core::GUID guid, const void* data, size_t size, Pitaya::Import::SkyBoxImportResult& out);
}
