#include"Utility.h"

#include<chrono>

std::string Pitaya::Core::Utility::Date()
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