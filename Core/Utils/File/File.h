#pragma once

#include<string>
#include<fstream>
#include<filesystem>

namespace Pitaya::Core
{
	std::string GetExeDir();

	bool ReadFileToString(const std::filesystem::path&, std::string&);
	bool IsSubPath(const std::filesystem::path& path, const std::filesystem::path& base);
}
