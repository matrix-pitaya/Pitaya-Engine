#include<Core/Utils/String/String.h>

#include<algorithm>

std::string Pitaya::Core::ToLower(std::string_view strview) noexcept
{
	std::string str = std::string(strview);
	std::transform(str.begin(), str.end(), str.begin(),
		[](unsigned char c) { return std::tolower(c); });
	return str;
}
std::string Pitaya::Core::ToUpper(std::string_view strview) noexcept
{
	std::string str = std::string(strview);
	std::transform(str.begin(), str.end(), str.begin(),
		[](unsigned char c) { return std::toupper(c); });
	return str;
}
void Pitaya::Core::ToLower(std::string& str) noexcept
{
	std::transform(str.begin(), str.end(), str.begin(),
		[](unsigned char c) { return std::tolower(c); });
}
void Pitaya::Core::ToUpper(std::string& str) noexcept
{
	std::transform(str.begin(), str.end(), str.begin(),
		[](unsigned char c) { return std::toupper(c); });
}
