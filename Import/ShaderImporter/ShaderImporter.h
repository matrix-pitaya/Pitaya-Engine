#pragma once

#include<Import/Common/ImportResult.h>

#include<filesystem>

namespace Pitaya::Import
{
	class ShaderImporter
	{
	public:
		ShaderImporter() = default;
		~ShaderImporter() = default;

	public:
		bool Import(Pitaya::Core::GUID guid, const std::filesystem::path& folder, const std::filesystem::path& vertexPath, const std::filesystem::path& fragmentPath, ShaderImportResult& out);
		bool Import(Pitaya::Core::GUID guid, const std::filesystem::path& folder, const std::filesystem::path& vertexPath, const std::filesystem::path& fragmentPath, const std::filesystem::path& geometryPath, ShaderImportResult& out);
	};
}
