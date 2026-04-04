#include<Core/Utils/File/File.h>

#include<windows.h>

std::filesystem::path Pitaya::Core::GetExecutableDirectory()
{
	char path[MAX_PATH] = { 0 };
	GetModuleFileNameA(NULL, path, MAX_PATH);
	std::filesystem::path exePath(path);
	return exePath.parent_path();
}
bool Pitaya::Core::ReadFileToString(const std::filesystem::path& filepath, std::string& out)
{
	std::ifstream file(filepath.c_str(), std::ios::binary | std::ios::ate);
	if (!file) { return false; }

	std::streamsize size = file.tellg();
	file.seekg(0, std::ios::beg);

	out.resize(static_cast<size_t>(size));
	file.read(out.data(), size);

	return file.good();
}
bool Pitaya::Core::IsSubPath(const std::filesystem::path& path, const std::filesystem::path& base)
{
	std::filesystem::path absPath = std::filesystem::absolute(path);
	std::filesystem::path absBase = std::filesystem::absolute(base);

	auto parent = absPath.parent_path();
	while (parent != parent.parent_path())
	{
		if (std::filesystem::equivalent(parent, absBase)) { return true; }
		parent = parent.parent_path();
	}
	return false;
}
