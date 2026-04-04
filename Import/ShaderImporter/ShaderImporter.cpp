#include<Import/ShaderImporter/ShaderImporter.h>
#include<GPU/Frontend/Shader/Shader.h>
#include<Core/Utils/File/File.h>
#include<Log/Common/FuncTable.h>

bool Pitaya::Import::ShaderImporter::Import(Pitaya::Core::GUID guid, const std::filesystem::path& folder, const std::filesystem::path& vertexPath, const std::filesystem::path& fragmentPath, Pitaya::Import::ShaderImportResult& out)
{
	if (vertexPath.empty() || fragmentPath.empty())
	{
		Pitaya::Log::Error(guid.ToString() + "shader vertex | fragment path is empty in folder:" + folder.string());
		return false;
	}

	if (!Pitaya::Core::ReadFileToString(vertexPath, out.VertexSource))
	{
		Pitaya::Log::Error("vertex shader load fail path:" + vertexPath.string());
		return false;
	}
	if (!Pitaya::Core::ReadFileToString(fragmentPath, out.FragmentSource))
	{
		Pitaya::Log::Error("fragment shader load fail path:" + fragmentPath.string());
		return false;
	}

	out.GUID = guid;
	out.Type = Pitaya::GPU::Shader::VF;
	return true;
}
bool Pitaya::Import::ShaderImporter::Import(Pitaya::Core::GUID guid, const std::filesystem::path& folder, const std::filesystem::path& vertexPath, const std::filesystem::path& fragmentPath, const std::filesystem::path& geometryPath, Pitaya::Import::ShaderImportResult& out)
{
	if (vertexPath.empty() || fragmentPath.empty() || geometryPath.empty())
	{
		Pitaya::Log::Error(guid.ToString() + "shader vertex | fragment | geometry path is empty in folder:" + folder.string());
		return false;
	}

	if (!Pitaya::Core::ReadFileToString(vertexPath, out.VertexSource))
	{
		Pitaya::Log::Error("vertex shader load fail path:" + vertexPath.string());
		return false;
	}
	if (!Pitaya::Core::ReadFileToString(fragmentPath, out.FragmentSource))
	{
		Pitaya::Log::Error("fragment shader load fail path:" + fragmentPath.string());
		return false;
	}
	if (!Pitaya::Core::ReadFileToString(geometryPath, out.GeometrySource))
	{
		Pitaya::Log::Error("geometry shader load fail path:" + geometryPath.string());
		return false;
	}

	out.GUID = guid;
	out.Type = Pitaya::GPU::Shader::VFG;
	return true;
}

