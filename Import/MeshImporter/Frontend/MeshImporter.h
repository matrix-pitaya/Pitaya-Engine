#pragma once

#include<Import/Common/ImportResult.h>

#include<filesystem>

namespace Pitaya::Import
{ 
	class MeshImporter
	{
	public:
		MeshImporter() = default;
		virtual ~MeshImporter() = default;

	public:
		virtual bool Import(Pitaya::Core::GUID guid, const std::filesystem::path& file, MeshPreloadResult& out) = 0;
		virtual bool Import(Pitaya::Core::GUID guid, const std::filesystem::path& file, const MeshPreloadResult& preload, StaticMeshImportResult& out) = 0;
		virtual bool Import(Pitaya::Core::GUID guid, const std::filesystem::path& file, const MeshPreloadResult& preload, SkinnedMeshImportResult& out) = 0;
	};
}
