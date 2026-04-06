#pragma once

#include<string>
#include<fstream>
#include<filesystem>

#define NOMINMAX
#include<windows.h>
#include<stdlib.h>

namespace Pitaya::Core
{
	inline std::filesystem::path GetExecutableDirectory()
	{
		char path[MAX_PATH] = { 0 };
		GetModuleFileNameA(NULL, path, MAX_PATH);
		std::filesystem::path exePath(path);
		return exePath.parent_path();
	}
	inline std::filesystem::path GetWorkspace()
	{
		return __argc <= 1 ? 
			std::filesystem::absolute(__argv[0]).parent_path() / "workspace" : 
			std::filesystem::absolute(__argv[1]).parent_path();
	}
	inline bool GenerateFile(const std::filesystem::path& directory, const char* filename, const char* title,const char* info)
	{
		const std::filesystem::path path = directory / filename;
		std::ofstream ofs(path, std::ios::out | std::ios::trunc);
		if (!ofs.is_open())
		{
			MessageBoxA(NULL, ("Generate File Fail! Path: " + path.string()).c_str(), "Error", MB_OK);
			return false;
		}
		SYSTEMTIME st;
		GetLocalTime(&st);
		ofs << "========================================\n";
		ofs << title << "\n";
		ofs << "Generated: " << st.wYear << "-" << st.wMonth << "-" << st.wDay << " "
			<< st.wHour << ":" << st.wMinute << ":" << st.wSecond << "\n";
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
