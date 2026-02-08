#pragma once

#include<Identifier/UID.h>

#include<string>
#include<chrono>
#include<windows.h>

namespace Pitaya::Core::Utility
{
	inline std::string Date()
	{
		auto t = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
		struct tm buf;
#if defined(_WIN32)
		localtime_s(&buf, &t);
#else
		localtime_r(&t, &buf);
#endif
		char temp[32] = {};
		std::strftime(temp, sizeof(temp), "%Y-%m-%d %H:%M:%S", &buf);
		return temp;
	}
	inline std::string GetExeDir()
	{
#ifdef _WIN32
		char path[MAX_PATH] = { 0 };
		GetModuleFileNameA(NULL, path, MAX_PATH);
		std::string fullPath(path);
		size_t pos = fullPath.find_last_of("\\/");
		return (std::string::npos == pos) ? "" : fullPath.substr(0, pos + 1);
#else
		char path[1024] = { 0 };
		ssize_t count = readlink("/proc/self/exe", path, sizeof(path));
		if (count == -1) return "";
		std::string fullPath(path, count);
		size_t pos = fullPath.find_last_of("/");
		return (std::string::npos == pos) ? "" : fullPath.substr(0, pos + 1);
#endif
	}
}