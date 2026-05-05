#pragma once

#include<string>
#include<fstream>
#include<filesystem>
#include<stdlib.h>
#include<chrono>

#if defined(PITAYA_PLATFORM_WINDOWS)
#define NOMINMAX
#include<windows.h>
#endif

namespace Pitaya::Core
{
	inline std::filesystem::path GetExecutableDirectory()
	{
#if defined(PITAYA_PLATFORM_WINDOWS)
		char path[MAX_PATH] = { 0 };
		GetModuleFileNameA(NULL, path, MAX_PATH);
		std::filesystem::path exePath(path);
		return exePath.parent_path();
#endif
	}
	inline std::filesystem::path GetWorkspace()
	{
#if defined(PITAYA_PLATFORM_WINDOWS)
		return __argc <= 1 ? 
			std::filesystem::absolute(__argv[0]).parent_path() / "workspace" : 
			std::filesystem::absolute(__argv[1]).parent_path();
#endif
	}
	inline bool GenerateFile(const std::filesystem::path& directory, const char* filename, const char* title,const char* info)
	{
		const std::filesystem::path path = directory / filename;
		std::ofstream ofs(path, std::ios::out | std::ios::trunc);
		if (!ofs.is_open()) { return false; }
		ofs << "========================================\n";
		ofs << title << "\n";
		auto t = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
		struct tm buf;
		localtime_s(&buf, &t);
		char temp[32] = {};
		std::strftime(temp, sizeof(temp), "%Y-%m-%d %H:%M:%S", &buf);
		ofs << "Generated: " << temp << "\n";
		ofs << "========================================\n\n";
		ofs << info;
		ofs.flush();
		ofs.close();
		return true;
	}
	inline bool ReadFileToString(const std::filesystem::path& filepath, std::string& out)
	{
		std::ifstream file(filepath.c_str(), std::ios::binary | std::ios::ate);
		if (!file) { return false; }

		std::streamsize size = file.tellg();
		file.seekg(0, std::ios::beg);

		out.resize(static_cast<size_t>(size));
		file.read(out.data(), size);

		return file.good();
	}
	inline bool IsSubPath(const std::filesystem::path& path, const std::filesystem::path& base)
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
}
