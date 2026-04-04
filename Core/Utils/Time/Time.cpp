#include<Core/Utils/Time/Time.h>

std::string Pitaya::Core::Date()
{
	auto t = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
	struct tm buf;
	localtime_s(&buf, &t);
	char temp[32] = {};
	std::strftime(temp, sizeof(temp), "%Y-%m-%d %H:%M:%S", &buf);
	return temp;
}
std::string Pitaya::Core::Data(std::chrono::system_clock::time_point tp)
{
	auto t = std::chrono::system_clock::to_time_t(tp);
	struct tm buf;
	localtime_s(&buf, &t);
	char temp[32] = {};
	std::strftime(temp, sizeof(temp), "%Y-%m-%d %H:%M:%S", &buf);
	return temp;
}
std::string Pitaya::Core::Data(time_t tp)
{
	struct tm buf;
	localtime_s(&buf, &tp);
	char temp[32] = {};
	std::strftime(temp, sizeof(temp), "%Y-%m-%d %H:%M:%S", &buf);
	return temp;
}
