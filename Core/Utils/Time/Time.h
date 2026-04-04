#pragma once

#include<string>
#include<chrono>

namespace Pitaya::Core
{
	std::string Date();
	std::string Data(std::chrono::system_clock::time_point);
	std::string Data(time_t);
}
